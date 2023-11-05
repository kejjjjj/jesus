#pragma once

#include "pch.hpp"

class EvarBase {
public:
    virtual ~EvarBase() = default;
    virtual std::string get_name() const = 0;
    virtual bool is_array() const = 0;
    virtual void* get_raw() const = 0;
};

template<typename T>
class Evar : public EvarBase {
public:
    Evar(const std::string& name) : name(name), data(nullptr) {}

    Evar(const std::string& name, const T& value) : name(name), data(new T(value)) {
        *(T*)get_raw() = value;

    }

    Evar(const std::string& name, const T* array, size_t size) : name(name), data(new T[size]) {
        isArray = true;
        for (size_t i = 0; i < size; ++i) {
            data[i] = array[i];
        }
    }

    ~Evar() = default;

    void* get_raw() const override {
        return reinterpret_cast<void*>(&get());
    }

    T& get() const noexcept {
        if (!data) {
            FatalError("Evar is not initialized");
        }
        return *data.get();
    }

    T* get_array() const noexcept {
        if (!data) {
            FatalError("Evar is not initialized");
        }
        return data.get();
    }

    std::string get_name() const override {
        return name;
    }

    bool is_array() const override {
        return isArray;
    }

private:
    std::string name;
    std::unique_ptr<T[]> data;
    bool isArray = false;
};

class EvarTable {
public:
    static EvarTable& getInstance() {
        static EvarTable instance;
        return instance;
    }

    template<typename T>
    decltype(auto) [[maybe_unused]] add_variable(const std::string& name, const T& value) {
        variables.push_back(std::make_unique<Evar<T>>(name, value));

        return variables.back().get();
    }

    template<typename T>
    decltype(auto) [[maybe_unused]] add_array(const std::string& name, const T* array, const size_t size) {
        variables.push_back(std::make_unique<Evar<T>>(name, array, size));

        return variables.back().get();

    }

    template<typename T>
    Evar<T>* find(const std::string& name) {
        for (const auto& var : variables) {
            Evar<T>* dynVar = dynamic_cast<Evar<T>*>(var.get());
            if (dynVar && dynVar->get_name().compare(name) == false) {
                return dynVar;
            }
        }
        return nullptr;
    }

private:
    EvarTable() = default;
    ~EvarTable() = default;
    EvarTable(const EvarTable&) = delete;
    EvarTable& operator=(const EvarTable&) = delete;

    std::list<std::unique_ptr<EvarBase>> variables;
};

template<typename T>
decltype(auto) find_evar(const std::string& name) {
    static EvarTable& instance = EvarTable::getInstance();
    decltype(auto) result = instance.find<T>(name);

    if (!result)
        FatalError(std::format("find_evar(): didn't find \"{}\"\ninstead of handling the error, the game will just close because this shouldn't happen anyway!", name));

    return result;
}
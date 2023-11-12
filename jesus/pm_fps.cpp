#include "pch.hpp"

void swapForAxis(fps_zone& zone)
{
	if (AngularDistance(zone.end, 0) > 50 && AngularDistance(zone.start, 0) > 45) {
		zone.start = -AngularDistance(zone.start, 90);
		zone.end = AngularDistance(zone.end, -90);
		return;

	}
	else if (AngularDistance(zone.end, 0) > 45) {
		zone.start = AngularDistance(zone.start, -90);
		zone.end = AngularDistance(zone.end, -90);
		return;
	}



	zone.start = AngularDistance(zone.start, -90);
	zone.end = AngularDistance(zone.end, -90);

	return;
}

std::vector<fps_zone> FPS_GetZones(int g_speed)
{
	std::vector<int> fps;
	std::vector<fps_zone> zone;

	zone.clear();

	//note: g_speed will mess these hardcoded values up!
	fps.push_back(333);
	fps.push_back(250);
	fps.push_back(200);
	fps.push_back(125);
	//fps.push_back(111);
	//fps.push_back(100);


	//use this if you need g_speed

	//for (int i = 2; i < 5; i++) {

	//	float v = std::round(float(g_speed) / (1000.f / i));
	//	if (v == 0.f)
	//		continue;

	//	if (fps.empty() == false)
	//		if (fps.back() == 1000 / i)
	//			continue;

	//	fps.push_back(1000 / i);



	//}

	fps_zone copy;

	zone.resize(fps.size());
	zone[0].start = (std::round(g_speed / (1000 / fps[0])));
	zone[0].end = -zone[0].start;

	zone[0].FPS = fps[0];

	if (g_speed == 190) {
		zone[0].start -= 1;
		zone[0].end = -zone[0].start;
	}

	if (find_evar<bool>("Long 125")->get()) {
		zone[0].start += 17.f;
	}

	zone[0].start = int(zone[0].start);
	zone[0].end = int(zone[0].end);

	copy = zone[0];
	swapForAxis(copy);

	zone[0].length = (zone[0].start > 45 ? 90.f - zone[0].start : zone[0].start) * 2;


	for (int i = 1; i < fps.size(); i++) {

		zone[i].start = zone[i - 1].end;
		zone[i].end = zone[i].start * float(1000 / fps[i - 1]) / float(1000 / fps[i]);

		if (find_evar<bool>("Long 125")->get() && fps[i] == 125) {
			zone[i].end = zone[0].start;
		}

		zone[i].start = int(zone[i].start);
		zone[i].end = int(zone[i].end);
		zone[i].FPS = fps[i];
		zone[i].length = (zone[i].start > 45 ? 90.f - zone[i].start : zone[i].start) * 2;


	}

	for (auto& z : zone) {
		std::cout << std::format("{}: [{:.1f}, {:.1f}]\n", z.FPS, z.start, z.end);
	}

	return zone;
}

int32_t FPS_GetIdeal(playerState_s* ps, usercmd_s* cmd)
{
	const auto in_range = [&cmd](float yaw, float min, float max, bool info) -> bool {
		const float screen_center = float(cgs->refdef.width / 2);

		const float aa = RAD2DEG(atan2(-(int)cmd->rightmove, (int)cmd->forwardmove));
		yaw = AngleNormalize90(yaw + aa);
		const float fov = Dvar_FindMalleableVar("cg_fov")->current.value * Dvar_FindMalleableVar("cg_fovscale")->current.value;

		range_t results = AnglesToRange(min, max, yaw, fov);

		return results.x1 <= screen_center && results.x2 >= screen_center;
	};
	static int g_speed = 0;
	static std::vector<fps_zone> zones = FPS_GetZones(ps->speed);
	fps_zone copy;
	if (g_speed != ps->speed || fps::refresh_required) {
		zones = FPS_GetZones(ps->speed);
		g_speed = ps->speed;
		fps::refresh_required = false;
	}

	copy = zones[0];


	if (cmd->rightmove == 127) {
		copy.start *= -1;
		copy.end *= -1;

	}

	if (!in_range(ps->viewangles[YAW], copy.start, copy.end, 0))
		return 333;

	swapForAxis(copy);

	if (in_range(ps->viewangles[YAW], copy.start, copy.end, 0))
		return 333;


	for (int zone = 1; zone < zones.size(); zone++) {

		copy = zones[zone];

		if (cmd->rightmove == 127) {
			copy.start *= -1;
			copy.end *= -1;

		}

		if (in_range(ps->viewangles[YAW], copy.start, copy.end, 0))
			return zones[zone].FPS;

		swapForAxis(copy);

		if (in_range(ps->viewangles[YAW], copy.start, copy.end, 0))
			return zones[zone].FPS;

	}



	return 333;
}

zone_distance FPS_GetDistanceToZone(playerState_s* ps, usercmd_s* cmd, int wishFPS)
{
	static int g_speed = 0;
	static std::vector<fps_zone> zones = FPS_GetZones(ps->speed);
	if (g_speed != ps->speed || fps::distances_refresh_required) {
		zones = FPS_GetZones(ps->speed);
		g_speed = ps->speed;
		fps::distances_refresh_required = false;
	}

	fps_zone* zone = 0;

	std::vector<fps_zone>::iterator it = (std::find_if(zones.begin(), zones.end(), [wishFPS](const fps_zone& z) {
		return (wishFPS == z.FPS); }));

	if (it == zones.end())
		FatalError("GetFPSZoneDistances didn't find matching fps");

	zone = &*it;

	char rightmove = cmd->rightmove;

	if (rightmove >= 0)
		rightmove *= -1;

	const float yaw = AngleNormalize90(ps->viewangles[YAW] + RAD2DEG(atan2(-(int)rightmove, (int)cmd->forwardmove))); //yaw + accel angle
	const float real_yaw = AngleNormalize90(ps->viewangles[YAW]);;
	zone_distance results;

	results.begin = AngleNormalize90((AngleDelta(yaw, zone->end)));
	results.end = AngleNormalize90((AngleDelta(yaw, zone->start)));

	if (real_yaw >= 0) {
		results.begin *= -1;
	}
	else {
		results.begin = 90.f - results.begin;
		results.end += 90.f;

		if (results.begin >= 90)
			results.begin -= 180;
		if (results.end >= 90)
			results.end -= 180;
	}

	if (results.begin < 0)
		results.begin += 90;
	if (results.end < 0)
		results.end += 90;

	results.length = zone->length;

	return results;
}
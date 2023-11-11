#include "pch.hpp"

std::vector<elebot_detection_data> elebot_surfaces;

elebot_detection_data* elebot = 0;
bool trying_to_find_edge = false;
bool playback_in_progress = false;
Playback playback;

constexpr int LINEUP_FPS = 333;


void elebot_save_selected()
{
	if (NOT_SERVER)
		return;

	if (!elebot || elebot->is_valid == false || elebot->playback.empty()) {
		return Com_Printf("^1no valid elevator surface selected\n");
	}

	const char* current_map = Dvar_FindMalleableVar("mapname")->current.string;
	std::string path = ("elebot");

	CG_CreateSubdirectory(path);
	std::string real_path = fs::root_path() + "\\" + path;
	std::string file_path = real_path + "\\" + current_map + ".air";

	_fs::remove(file_path);

	if (fs::file_exists(file_path)) {
		return Com_Printf("^1cannot save the elevator because the file is already in use by something else\n%s\n", file_path.c_str());
	}

	fs::create_file(file_path);

	std::ofstream o(file_path, static_cast<int>(fs::fileopen::FILE_OUT));

	if (!o.is_open()) {
		FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", real_path, fs::get_last_error()));
		return;
	}

	for (auto& eb : elebot_surfaces) {
		elebot_export_data elebot_export;

		elebot_export.playback = eb.playback;
		elebot_export.normals = eb.normals;
		elebot_export.hitAxis = eb.hitAxis;
		elebot_export.destination = eb.destination;
		elebot_export.lineup_trigger = eb.lineup_trigger;
		elebot_export.target_yaw = eb.target_yaw;
		elebot_export.brush_origin = eb.brush->get_origin();

		//IO_WriteData<integer>(o, integer(elebot_export.playback->size()));

		o << elebot_export.playback.size();

		for (auto& i : elebot_export.playback) {
			IO_WriteData<playback_cmd>(o, i);
		}
		o << '\n';
#pragma warning (suppress : 6305)
		IO_WriteData(o, &elebot_export.normals, sizeof(elebot_export_data) - sizeof(elebot_export.playback));

		eb.saved = true;
	}

	o.close();

}
void elebot_load_from_map(const char* mapname)
{
	elebot_surfaces.clear();

	std::string path = ("elebot");

	std::string real_path = fs::root_path() + "\\" + path;
	std::string file_path = real_path + "\\" + mapname + ".air";

	if (!fs::file_exists(file_path))
		return;

	fs::reset();

	std::fstream f(file_path, static_cast<std::ios_base::openmode>(fs::fileopen::FILE_IN));

	if (!f.is_open()) {
		FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", file_path, fs::get_last_error()));
		return;
	}
	while (f.good() && !f.eof()) {

		elebot_export_data data;

		std::string v;
		char ch = fs::get(f); //
		int num_inputs = 0;

		if ((f.good() && !f.eof()) == false)
			break;

		while (ch != '[') {
			v.push_back(ch);
			ch = fs::get(f);

		}

		try {
			num_inputs = std::stoi(v);
		}
		catch (...) {
			Com_Printf("^1syntax error in file\n");
			break;
		}

		std::cout << "numinputs: " << num_inputs << '\n';

		for (int i = 0; i < num_inputs && f.good() && !f.eof(); i++) {
			data.playback.push_back(IO_ReadBlock<playback_cmd>(f).value());
		}

		fs::get(f);


		if (fs::file.current_character != '\n') {
			Com_Printf("^1expected a newline, but got (%c)\n", fs::file.current_character);
			break;
		}
			

		IO_ReadBlock(f, &data.normals, sizeof(elebot_export_data) - sizeof(data.playback));
		//fs::get(f);

		elebot_detection_data d;

		d.brush = CM_FindBrushByOrigin(data.brush_origin);
		d.brush_origin = data.brush_origin;
		d.destination = data.destination;
		d.hitAxis = data.hitAxis;
		d.lineup_trigger = data.lineup_trigger;
		d.normals = data.normals;
		d.playback = data.playback;
		d.target_yaw = data.target_yaw;
		d.is_valid = true;

		//std::optional<sc_winding_t> w = CM_GetBrushWinding(d.brush, d.normals);

		//if (!w) {
		//	Com_Printf("^1couldn't find winding for brush\n");
		//	break;
		//}

		//d.face = w.value();

		elebot_surfaces.push_back(d);
		elebot = &elebot_surfaces.back();
		elebot->saved = true;
	}

	f.close();

}
void give_random_inputs(prediction_controller& pc, const float yaw)
{
	pc.forwardmove = bool(std::round(random(1.f))) ? 127 : -127;
	pc.rightmove = bool(std::round(random(1.f))) ? 127 : -127;

	if(!pc.turntype)
		pc.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle(0.f, (yaw - random(90.f)), 0.f)));

	pc.FPS = 333;
	pc.buttons = cmdEnums::crouch;
}

void reset_prediction(pmove_t* pm, float origin, const fvec3& velocity, Axis_t axis) 
{
	pm->ps->origin[int(axis)] = origin;
	pm->ps->origin[2] = 99999.f;
	VectorCopy(velocity, pm->ps->velocity);
}

float predict_next_position(pmove_t* pm, pml_t* pml, Axis_t axis, float yaw, char forwardmove, char rightmove) 
{
	prediction_controller c;
	c.buttons = cmdEnums::crouch;
	c.forwardmove = forwardmove;
	c.rightmove = rightmove;
	c.FPS = LINEUP_FPS;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle(0.f, yaw, 0.f)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_TURN;

	PmoveSingleSimulation(pm, pml, &c);
	
	return pm->ps->origin[int(axis)];

}

void setup_player(pmove_t* pm, pml_t* pml, Axis_t axis, float origin, float destination, float* normals) 
{
	playerState_s* ps = pm->ps;

	VectorClear(ps->velocity);
	ps->origin[int(axis)] = origin;
	ps->origin[2] = 99999.f; //go far up in the sky that we HAVE to be in the air

	float yaw = -(atan2(normals[1], normals[0]) * 180.f / PI);

	if (axis == Axis_t::X)
		yaw = AngleNormalize180(yaw + 180);

	ps->oldVelocity[0] = 0;
	ps->oldVelocity[1] = 0;

	prediction_controller c;
	c.buttons = cmdEnums::crouch;
	c.forwardmove = 0;
	c.rightmove = 0;
	c.FPS = LINEUP_FPS;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle(0, yaw, 0)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;

	PmoveSingleSimulation(pm, pml, &c);
	PmoveSingleSimulation(pm, pml, &c);
	
}
float slowdown_position(pmove_t* pm, pml_t* pml, Axis_t axis, float origin, float destination, float yaw)
{
	float newOrigin = 0;
	int lowest_velocity = int(std::fabs(pm->ps->velocity[int(axis)]));

	while(true){

		newOrigin = predict_next_position(pm, pml, axis, yaw, -127, 0);

		int vel = int(std::fabs(pm->ps->velocity[int(axis)]));

		if (vel <= lowest_velocity && vel != 0)
			lowest_velocity = vel;
		else
			break;
	}

	//float delta = std::fabs(newOrigin - origin);

	return newOrigin;

}
void elebot_evaluate_angles_midair(playerState_s* ps)
{
	

	elebot_detection_data _elebot;

	fvec3 start = rg->viewOrg;
	fvec3 end = fvec3(rg->viewOrg) + (fvec3(ps->viewangles).toforward() * 99999);
	trace_t trace;
	float distance_moved = 0.f;


	CG_TracePoint(vec3_t{ 1,1,1 }, &trace, start, vec3_t{ -1,-1,-1 }, end, ps->clientNum, MASK_PLAYERSOLID, 0, 0);


	if (fabs(trace.normal[0]) != 1.f && fabs(trace.normal[1]) != 1.f) {
		Com_Printf("invalid surface\n");
		return;
	}

	if (elebot && !elebot->saved) {
		elebot_surfaces.pop_back();
		elebot = &elebot_surfaces.back();
	}

	Axis_t hitAxis = fabs(trace.normal[0]) == 1.f ? Axis_t::X : Axis_t::Y;

	fvec3 hitpos = start + ((end - start) * trace.fraction);
	fvec3 first_origin = ps->origin;

	hitpos += (fvec3(trace.normal)) * 14;
	const float start_origin = hitpos[int(hitAxis)];
	hitpos += (fvec3(trace.normal).inverse()) * 0.125f;
	const float destination = hitpos[int(hitAxis)];
	const float total_distance = fabs(destination - start_origin);

	//Com_Printf("hit: %.6f\n", hitpos[int(hitAxis)]);

	pmove_t pm = PM_Create(&cgs->predictedPlayerState, cinput->GetUserCmd(cinput->currentCmdNum), cinput->GetUserCmd(cinput->currentCmdNum - 1));
	pml_t pml = PML_Create(&pm, 333);
	playerState_s _ps;
	memcpy(&_ps, pm.ps, sizeof(playerState_s));
	pm.ps = &_ps;

	setup_player(&pm, &pml, hitAxis, start_origin, destination, trace.normal);

	float yaw = pm.ps->viewangles[YAW];
	float yaw_delta = 0.f;

	fvec3 old_velocity = (0,0,0);
	float origin = start_origin;

	char forwardmove = 127;
	char rightmove = 0;

	int successful_frames = 0;

	bool gg = false;

	float closest_distance = total_distance;

	size_t numTries = 0;
	int lowest_inputs = 30;
	int prediction_fails = 0;

	while (numTries++ != 10000) {
		std::list<playback_cmd> current_attempt;
		float oldOrigin = origin;
		distance_moved = 0.f;
		VectorCopy(first_origin, pm.ps->origin);

		reset_prediction(&pm, origin, old_velocity, hitAxis);

		for (int i = 0; i < 30; i++) {

			prediction_controller c;
			give_random_inputs(c, yaw);

			float newOrigin = predict_next_position(&pm, &pml, hitAxis, dynamic_cast<prediction_viewangle_fixed_angle*>(c.turntype.get())->right, c.forwardmove, c.rightmove);
			
			current_attempt.push_back(cmd2playback(pm.ps, &pm.cmd, c.FPS));

			float delta = fabs(newOrigin - oldOrigin);

			distance_moved += delta;

			if (i == 29 && distance_moved == 0.f)
				return Com_Printf("^1for some reason the bot is unable to move at high coordinates\n");

			if (delta != 0)
				oldOrigin = newOrigin;

			if (distance_moved > total_distance) {
				break;
			}


			if (total_distance - distance_moved < closest_distance)
				closest_distance = total_distance - distance_moved;

			else if (newOrigin == destination) {

				if (i < lowest_inputs) {
					lowest_inputs = i;
					_elebot.playback = current_attempt;
				}
				successful_frames++;

				Com_Printf("found the correct inputs with %i inputs\n", i);
				gg = true;
				break;
			}

		}
	}


	if (!gg) {
		Com_Printf("closest attempt was %.6f units away after %u attempts\n", closest_distance, numTries);
		return;
	}

	Com_Printf("best attempt from (%i) attempts had %i inputs and %i fails\n", successful_frames, lowest_inputs, prediction_fails);

	//return;

	//go a bit further into the wall to find the brush
	hitpos += (fvec3(trace.normal).inverse()) * 15.f;

	cbrush_t* b = CM_FindBrushByOrigin(hitpos);

	if (!b) {
		 Com_Printf("^1couldn't find the ele surface?! (%.6f, %.6f, %.6f)\n", hitpos.x, hitpos.y, hitpos.z);
		 return;

	}
	auto winding = CM_GetBrushWinding(b, trace.normal);

	if (!winding.has_value()) {
		Com_Printf("^1couldn't find the winding?!\n");
		return;

	}
	_elebot.face = winding.value();
	_elebot.destination = destination;
	_elebot.normals = trace.normal;
	_elebot.hitAxis = hitAxis;
	_elebot.is_valid = true;
	_elebot.brush = b;
	_elebot.lineup_trigger = start_origin;
	_elebot.target_yaw = yaw;

	elebot_surfaces.push_back(_elebot);

	Com_Printf("distance moved: %.6f in %i frames to position (%.6f)\n", distance_moved, successful_frames, pm.ps->origin[int(hitAxis)]);

	elebot = &elebot_surfaces.back();
}


void elebot_render_winding(GfxViewParms* viewParms)
{



	for (auto& eb : elebot_surfaces) {
		if (!eb.is_valid)
			continue;

		if (eb.face.points.empty() || !eb.brush) {

			if (!eb.brush) {
				eb.brush = CM_FindBrushByOrigin(eb.brush_origin);
			}
			else {

				auto w = CM_GetBrushWinding(eb.brush, eb.normals);

				if (w)
					eb.face = w.value();

			}


			continue;
		}
		else {

			cplane_s frustum_planes[6];
			CreateFrustumPlanes(viewParms, frustum_planes);


			if (CM_BrushInView(eb.brush, frustum_planes, 5))
				RB_DrawCollisionPoly(eb.face.points.size(), (float(*)[3])eb.face.points.data(), vec4_t{ 0,1,0,0.3f }, true);
		}
	}

}
bool elebot_has_lineup(playerState_s* ps, usercmd_s* cmd)
{

	if (trying_to_find_edge) {
		fvec3 angles = ps->viewangles;
		angles.y = elebot->target_yaw;
		CL_SetPlayerAngles(cmd, ps->delta_angles, angles);
		Dvar_FindMalleableVar("com_maxfps")->current.integer = LINEUP_FPS;
		return true;
	}

	if (!elebot || !elebot->is_valid || playback_in_progress)
		return false;

	float eye = CG_GetEyeHeight(ps);

	for (auto& eb : elebot_surfaces) {
		
		trying_to_find_edge = ps->groundEntityNum == 1023 && ps->origin[int(eb.hitAxis)] == eb.lineup_trigger &&
			eye > eb.brush->mins[2] && eye < eb.brush->maxs[2];

		if (trying_to_find_edge) {
			elebot = &eb;
			return true;
		}
	}
	return false;

}
void elebot_start_lineup(playerState_s* ps, usercmd_s* cmd)
{


	pmove_t pm = PM_Create(ps, cmd, cinput->GetUserCmd(cinput->currentCmdNum - 1));
	pml_t pml = PML_Create(&pm, LINEUP_FPS);
	playerState_s _ps;
	memcpy(&_ps, pm.ps, sizeof(playerState_s));
	pm.ps = &_ps;

	prediction_controller c;
	c.buttons = cmdEnums::crouch;
	c.forwardmove = 0;
	c.rightmove = 0;
	c.FPS = LINEUP_FPS;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle(0, elebot->target_yaw, 0)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;

	PmoveSingleSimulation(&pm, &pml, &c);
	PmoveSingleSimulation(&pm, &pml, &c); //fix yaw

	pm.ps->origin[2] = ps->origin[2];

	float newOrigin = predict_next_position(&pm, &pml, elebot->hitAxis, elebot->target_yaw, 127, 0);
	
	if (newOrigin != ps->origin[int(elebot->hitAxis)]) {
		//Com_Printf("^2pog\n");
		trying_to_find_edge = false;
		playback_in_progress = true;
		playback.SetPlayback(elebot->playback);
		playback.StartPlayback(cmd->serverTime, ps, cmd, elebot->playback.front(), true);

	}

	cmd->forwardmove = 0;
	cmd->rightmove = 0;

}
void elebot_start_playback(playerState_s* ps, usercmd_s* cmd)
{
	if (playback_in_progress == false)
		return;

	if(playback.isPlayback())
		playback.doPlayback(cmd);

	else{
		cmd->buttons = 0;
		playback_in_progress = false;
		Com_Printf("^2stand up at %.6f\n", ps->origin[int(elebot->hitAxis)]);

		CBuf_Addtext("+gostand; wait; wait; -gostand");

		//CL_SetPlayerAngles(cmd, ps->delta_angles, elebot->playback.back().viewangles);
	}
	
	//else if (playback.isPlayback() == false) {
	//	Com_Printf("^1accuracy fail by %.6f units\n", std::fabs(ps->origin[int(elebot->hitAxis)]) - elebot->destination);
	//	playback_in_progress = false;
	//}

}
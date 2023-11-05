#include "pch.hpp"

bool BouncePrediction::RealTimePrediction(playerState_s* ps, usercmd_s* cmd)
{
	static float old_yaw = ps->viewangles[YAW];

	if (ps->groundEntityNum == 1022 || ps->groundEntityNum == 1023 && (ps->pm_flags & PMF_JUMPING) == 0)
		return false;

	//auto ps_copy = *ps;

	pmove_t pm = PM_Create(ps, cmd, CL_GetUserCmd(clients->cmdNumber - 1));
	pml_t pml = PML_Create(&pm, 333);

	prediction_controller c;
	c.buttons = pm.cmd.buttons;
	c.forwardmove = pm.cmd.forwardmove;
	c.rightmove = pm.cmd.rightmove;
	c.numInputs = c.FPS;
	float yaw_delta = std::clamp(-(AngleDelta(ps->viewangles[YAW], old_yaw)), -0.1f, 0.1f);
	
	if (ps->viewangles[YAW] != old_yaw) {
		old_yaw = ps->viewangles[YAW];
	}
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn(yaw_delta, 0)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_TURN;

	//if(get_new_render)
	//	r_predicted_cmds.clear();


	simulation_results results;
	predicted_cmds.clear();

	for (int i = 0; i < c.numInputs; i++) {

		c.FPS = FPS_GetIdeal(pm.ps, &pm.cmd);
		c.numInputs = c.FPS/1.5f;

		float oldZ = pm.ps->velocity[2];

		results = PmoveSingleSimulation(&pm, &pml, &c);

		if (pm.ps->groundEntityNum == 1022 || fvec2(pm.ps->velocity) == 0.f)
			return false;

		if ((pm.ps->pm_flags & PMF_JUMPING) == 0 && pm.ps->velocity[2] > (abs(oldZ)) / 1.5f) {
			Com_Printf("^5bunce!\n");
			return true;
		}

		predicted_cmds.push_back(cmd2playback(pm.ps, &pm.cmd, c.FPS));

		//if (get_new_render)
		//	r_predicted_cmds.push_back(predicted_cmds.back());

	}

	//get_new_render = false;

	return false;

	

}

void BouncePrediction::PredictBounce(usercmd_s* cmd)
{
	if (find_evar<bool>("Easy Bounces")->get() == false)
		return;

	static bool playback_active = false;
	static bool stop_predicting = false;
	static bool success = false;

	bool bplayback = prediction_playback && prediction_playback->isPlayback();

	if (stop_predicting == false && !MovementRecorder::getInstance().playback)
		success = RealTimePrediction(&cgs->predictedPlayerState, cmd);

	if (success) {
		prediction_playback = std::unique_ptr<Playback>(new Playback(predicted_cmds));

		prediction_playback->StartPlayback(cmd->serverTime, &cgs->predictedPlayerState, cmd, predicted_cmds.front(), false);
		stop_predicting = true;
		success = false;


		auto current = prediction_playback->CurrentCmd();

		if (!current)
			return;

		float dist = current->origin.dist(clients->cgameOrigin);

		//Com_Printf("start dist: %.6f\n", dist);
		//bplayback = true;
	}

	if (bplayback) {
		prediction_playback->doPlayback(cmd);

		if (!prediction_playback->isPlayback()) {
			//Com_Printf("bunce!\n");
			prediction_playback.reset();
			Dvar_FindMalleableVar("com_maxfps")->current.integer = 333;
		}


	}

	if (cgs->predictedPlayerState.groundEntityNum == 1022 && !bplayback)
		stop_predicting = false;
	
}

//void BouncePrediction::RB_ShowPath()
//{
//
//	if (r_predicted_cmds.empty() || get_new_render)
//		return;
//
//	std::list<playback_cmd>::iterator it;
//
//	const Pixel col = generateRainbowColor();
//
//	//std::uint8_t c[4];
//
//	//((char(__fastcall*)(const float* in, uint8_t * out))0x493530)(vec4_t{ col.r / 255.f,col.g / 255.f,col.b / 255.f,0.3f }, c);
//
//	auto begin = r_predicted_cmds.begin();
//	auto end = r_predicted_cmds.end();
//
//	for (it = begin; it != end; it++) {
//
//		fvec3 _a = it->origin;
//		fvec3 _b = (++it)->origin;
//
//		if (it == r_predicted_cmds.end())
//			break;
//
//
//		//RB_AddDebugLine(vtx, true, a, b, c, vertCount);
//
//		//RB_DrawLines3D(lines, 3, vtx, true);
//		if(auto a = WorldToScreen(_a))
//			if(auto b = WorldToScreen(_b))
//				ImGui::GetBackgroundDrawList()->AddLine(a.value(), b.value(), col.packed(), 3.f);
//
//
//		//++lines;
//		//vertCount += 2;
//		//vtx.reserve(vtx.size() + 2);
//
//		--it;
//
//	}
//
//	get_new_render = true;
//
//}

fvec3 P_PredictStopPosition(usercmd_s* cmd)
{
	pmove_t pm = PM_Create(&cgs->predictedPlayerState, cmd, CL_GetUserCmd(clients->cmdNumber));
	pml_t pml = PML_Create(&pm, Dvar_FindMalleableVar("com_maxfps")->current.integer);

	prediction_controller c;

	c.buttons = 0;
	c.forwardmove = 0;
	c.rightmove = 0;

	c.numInputs = 1;

	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn));

	auto fixed_angle = dynamic_cast<prediction_viewangle_fixed_turn*>(c.turntype.get());

	fixed_angle->right = 0.f;
	fixed_angle->up = 0.f;

	simulation_results r;
	while (pm.ps->velocity[0] != 0.f || pm.ps->velocity[1] != 0.f || pm.ps->velocity[2] != 0.f) {
		c.forwardmove = 0;
		c.rightmove = 0;
		r = PmoveSingleSimulation(&pm, &pml, &c);

	}
	return pm.ps->origin;

}
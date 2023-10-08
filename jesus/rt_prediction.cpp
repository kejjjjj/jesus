#include "pch.hpp"

std::optional<std::list<playback_cmd>> RealTimePrediction(playerState_s* ps, usercmd_s* cmd)
{
	static float old_yaw = ps->viewangles[YAW];

	if (ps->groundEntityNum == 1022 || ps->groundEntityNum == 1023 && (ps->pm_flags & PMF_JUMPING) == 0)
		return std::nullopt;

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


	//Com_Printf("yaw: %.6f\n", yaw_delta);

	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn(yaw_delta, 0)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_TURN;

	simulation_results results;

	std::list<playback_cmd> cmds;

	for (int i = 0; i < c.numInputs; i++) {

		c.FPS = FPS_GetIdeal(&pm);
		c.numInputs = c.FPS/1.5f;

		float oldZ = pm.ps->velocity[2];

		results = PmoveSingleSimulation(&pm, &pml, &c);

		if (pm.ps->groundEntityNum == 1022 || fvec2(pm.ps->velocity) == 0.f)
			return std::nullopt;

		if ((pm.ps->pm_flags & PMF_JUMPING) == 0 && pm.ps->velocity[2] > (abs(oldZ)) / 1.5f) {
			Com_Printf("^5bunce!\n");
			return cmds;
		}

		cmds.push_back(cmd2playback(pm.ps, &pm.cmd, c.FPS));


	}

	return std::nullopt;

	

}

void CL_PredictBounce(usercmd_s* cmd)
{
	static dvar_s* kej_easyBounces = Dvar_FindMalleableVar("kej_easyBounces");

	if (kej_easyBounces && kej_easyBounces->current.enabled) {
		static bool playback_active = false;
		static bool stop_predicting = false;
		static std::list<playback_cmd>::iterator pb_it;
		static std::list<playback_cmd>::iterator pb_end;
		static std::optional<std::list<playback_cmd>> results;

		bool bplayback = prediction_playback && prediction_playback->isPlayback();

		if (stop_predicting == false && !MovementRecorder::getInstance().playback)
			results = RealTimePrediction(&cgs->predictedPlayerState, cmd);

		if (results) {
			pb_it = results.value().begin();
			prediction_playback = std::unique_ptr<Playback>(new Playback(results.value()));

			prediction_playback->StartPlayback(cmd->serverTime, &cgs->predictedPlayerState, cmd, *pb_it, false);
			stop_predicting = true;
			results = std::nullopt;


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
}
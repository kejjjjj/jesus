#include "pch.hpp"

void M_MovementCheats(usercmd_s* cmd)
{
	M_SuperSprint(cmd);

	//for (int i = 0; i < clients->snap.numClients; i++) {
	//	entity_s target(i);

	//	if (target.isMe())
	//		return;

	//	M_AutoKnife(cmd, &target);
	//}

}
void M_SuperSprint(usercmd_s* cmd)
{
	if (Dvar_FindMalleableVar("hack_superSprint")->current.enabled == false)
		return;

	fvec3 angles = clients->cgameViewangles;

	if (/*(cmd->buttons & cmdEnums::sprint) == 0 || */cmd->forwardmove != 127)
		return;

	if (cmd->rightmove == 0)
		cmd->rightmove = 127;

	CL_SetSilentAngles({ angles.x, angles.y, cmd->rightmove > 0 ? -90.f : 90.f });



}
void M_AutoKnife(usercmd_s* cmd, entity_s* target)
{
	if (Dvar_FindMalleableVar("hack_autoKnife")->current.enabled == false)
		return;

	entity_s self(cgs->clientNum);

	const float dist = self.getOrigin().dist(target->getOrigin());
	
	if (dist > 50)
		return;

	const float angle = (target->getOrigin() - fvec3(clients->cgameOrigin)).toangles().y;

	fvec3 angles = clients->cgameViewangles;

	CL_SetSilentAngles({ angles.x, angle, angles.z });
	if((cmd->buttons & cmdEnums::melee) == 0)
		cmd->buttons |= cmdEnums::melee;

	//Com_Printf("^2knife!\n");
}
#include "pch.hpp"

void CM_AdvanceAabbTree(CollisionAabbTree* aabbTree, cm_terrain* terrain)
{

	if (aabbTree->childCount) {
		auto child = &cm->aabbTrees[aabbTree->u.firstChildIndex];
		for (int i = 0; i < aabbTree->childCount; i++) {
			CM_AdvanceAabbTree(child, terrain);
			++child;
		}
		return;
	}
	const auto var = find_evar<char>("Filter##01");
	if (std::string(cm->materials[aabbTree->materialIndex].material).find(var->get_array()) == std::string::npos) {
		return;
	}

	CollisionAabbTreeIndex fChild = aabbTree->u;
	CollisionPartition* partition = &cm->partitions[fChild.firstChildIndex];

	int firstTri = partition->firstTri;



	if (firstTri < firstTri + partition->triCount)
	{

		int triIndice = 3 * firstTri;

		terrain->aabb = aabbTree;


		do {
			cm_triangle tri;
			tri.edge_walkable = CM_IsEdgeWalkable(2, firstTri);

			tri.a = cm->verts[cm->triIndices[triIndice]];
			tri.b = cm->verts[cm->triIndices[triIndice + 1]];
			tri.c = cm->verts[cm->triIndices[triIndice + 2]];

			//filter out some of the useless ones that are inside solid
			if (!tri.edge_walkable) {
				trace_t trace;
				CG_TracePoint(vec3_t{ 0,0,0 }, &trace, tri.a, vec3_t{ 0,0,0 }, tri.a, cgs->clientNum, MASK_PLAYERSOLID, 0, 0);

				if (trace.startsolid)
					tri.edge_walkable = true;

			}

			PlaneFromPoints(tri.plane, tri.a, tri.b, tri.c);


			if ((tri.plane[2]) >= 0.3f && (tri.plane[2]) <= 0.7f)
				tri.edge_walkable = true;

			terrain->tris.push_back(std::move(tri));


			++firstTri;
			triIndice += 3;

		} while (firstTri < partition->firstTri + partition->triCount);

	}

}
void CM_GetTerrainTriangles(cLeaf_t* leaf, const std::string& material_filter)
{
	if (!leaf)
		return;

	if (!leaf->collAabbCount)
		return;

	int aabbIdx = 0;
	vec3_t a, b, c;

	cm_terrain terrain{};
	terrain.leaf = leaf;
	do {
		CollisionAabbTree* aabb = &cm->aabbTrees[aabbIdx + leaf->firstCollAabbIndex];

		CM_AdvanceAabbTree(aabb, &terrain);

		++aabbIdx;
	} while (aabbIdx < leaf->collAabbCount);

	if(terrain.tris.empty() == false)
		cm_terrainpoints.push_back(std::move(terrain));

}

void CM_ShowTerrain(cm_terrain* terrain, struct cplane_s* frustumPlanes)
{
	uint8_t col[4];
	vec3_t tris[3];
	fvec3 center;
	std::vector<fvec3> points(3);
	int i = 2;

	const auto only_bounces = find_evar<bool>("Only Bounces")->get();
	const auto depth_test = find_evar<bool>("Depth Test")->get();
	const auto drawdist = find_evar<float>("Draw Dist")->get();

	bool unwalkable_edges = find_evar<bool>("Unwalkable Edges")->get();

	for (auto it = terrain->tris.begin(); it != terrain->tris.end(); ++it) {

		if ((it->plane[2] < 0.3f || it->plane[2] > 0.7f) && only_bounces) {
			if (!unwalkable_edges)
				continue;

			else if (it->edge_walkable)
				continue;
		}

		if (it->edge_walkable && unwalkable_edges)
			continue;

		//don't render if not visible
		if (!CM_TriangleInView(&*it, frustumPlanes, 5))
			continue;

		vec4_t c = { 0,1,1,0.3f };

		if (only_bounces) {
			float n = it->plane[2];

			if (n > 0.7f || n < 0.3f)
				n = 0.f;
			else
				n = 1.f - (n - 0.3f) / (0.7f - 0.3f);

			c[0] = 1.f - n;
			c[1] = n;
			c[2] = 0.f;
		}

		R_ConvertColorToBytes(c, col);


		points[0] = (it->a);
		points[1] = (it->b);
		points[2] = (it->c);

		center.x = { (points[0].x + points[1].x + points[2].x) / 3 };
		center.y = { (points[0].y + points[1].y + points[2].y) / 3 };
		center.z = { (points[0].z + points[1].z + points[2].z) / 3 };

		if (center.dist(clients->cgameOrigin) > drawdist)
			continue;

		RB_DrawPolyInteriors(3, points, col, true, depth_test);

	}

}
bool CM_TriangleInView(const cm_triangle* tris, struct cplane_s* frustumPlanes, int numPlanes)
{
	if (numPlanes <= 0)
		return 1;



	cplane_s* plane = frustumPlanes;
	int idx = 0;
	while ((BoxOnPlaneSide(tris->get_mins(), tris->get_maxs(), plane) & 1) != 0) {
		++plane;
		++idx;

		if (idx >= numPlanes)
			return 1;
	}

	return 0;
}
char CM_IsEdgeWalkable(int edgeIndex, int triIndex)
{
	char r{};
	__asm
	{
		mov ecx, triIndex;
		mov eax, edgeIndex;
		mov esi, 0x4EFCB0;
		call esi;
		mov r, al;
	}

	return r;
}
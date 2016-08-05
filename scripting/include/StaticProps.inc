#if defined _StaticProps_included
#endinput
#endif
#define _StaticProps_included


// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum SolidType_t
{
	SOLID_NONE			= 0,	// no solid model
	SOLID_BSP			= 1,	// a BSP tree
	SOLID_BBOX			= 2,	// an AABB
	SOLID_OBB			= 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW		= 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM		= 5,	// Always call into the entity for tests
	SOLID_VPHYSICS		= 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

enum SolidFlags_t
{
	FSOLID_CUSTOMRAYTEST		= 0x0001,	// Ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST		= 0x0002,	// Ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID			= 0x0004,	// Are we currently not solid?
	FSOLID_TRIGGER				= 0x0008,	// This is something may be collideable but fires touch functions
											// even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	FSOLID_NOT_STANDABLE		= 0x0010,	// You can't stand on this
	FSOLID_VOLUME_CONTENTS		= 0x0020,	// Contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED	= 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	FSOLID_USE_TRIGGER_BOUNDS	= 0x0080,	// Uses a special trigger bounds separate from the normal OBB
	FSOLID_ROOT_PARENT_ALIGNED	= 0x0100,	// Collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS	= 0x0200,	// This trigger will touch debris objects
	
	FSOLID_MAX_BITS	= 10
};

enum Collision_Group_t
{
	COLLISION_GROUP_NONE  = 0,
	COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
										// TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC,			// Generic NPC group
	COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
	COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE,		// Projectiles!
	COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code
	
	COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other
	
	LAST_SHARED_COLLISION_GROUP
};


// pass in an array, and it will be filled with static prop indexes
// the return value indicates how many indexes were actually stored (0 <= retval <= array_max)
native int GetAllStaticProps      (int[] array, int array_max);
native int GetAllStaticPropsInAABB(int[] array, int array_max, const float mins[3], const float maxs[3]);
native int GetAllStaticPropsInOBB (int[] array, int array_max, const float origin[3], const float extent1[3], const float extent2[3], const float extent3[3]);


// for testing: this uses sigsegv's own AABB filtering instead of directly calling Valve's GetAllStaticPropsInAABB
native int GetAllStaticPropsInAABB_Alt(int[] array, int array_max, const float mins[3], const float maxs[3]);


// pass in a static prop index, and get the requested information
// the return value indicates whether the request was successful (false if the prop index is bad, etc)
native bool StaticProp_GetMins          (int index, float mins[3]);                      // essentially: ICollideable::OBBMins()
native bool StaticProp_GetMaxs          (int index, float maxs[3]);                      // essentially: ICollideable::OBBMaxs()
native bool StaticProp_GetOrigin        (int index, float origin[3]);                    // essentially: ICollideable::GetCollisionOrigin()
native bool StaticProp_GetAngles        (int index, float angles[3]);                    // essentially: ICollideable::GetCollisionAngles()
native bool StaticProp_GetSolidType     (int index, SolidType_t& solid_type);            // essentially: ICollideable::GetSolid()
native bool StaticProp_GetSolidFlags    (int index, SolidFlags_t& solid_flags);          // essentially: ICollideable::GetSolidFlags()
native bool StaticProp_GetCollisionGroup(int index, Collision_Group_t& collision_group); // essentially: ICollideable::GetCollisionGroup()
native bool StaticProp_GetModelName     (int index, char[] name, int name_max);          // essentially: IVModelInfo::GetModelName(ICollideable::GetCollisionModel())


public Extension __ext_StaticProps = {
	name = "StaticProps",
	file = "StaticProps.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

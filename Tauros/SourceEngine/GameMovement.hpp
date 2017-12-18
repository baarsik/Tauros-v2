#pragma once

class C_CSPlayer;

enum Collision_Group_t
{
	COLLISION_GROUP_NONE = 0,
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

class CMoveData
{
public:

	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	bool m_bNoAirControl : 1;

	void *m_nPlayerHandle;    // edict index on server, client entity handle on client

	int m_nImpulseCommand;    // Impulse command issued.
	Vector m_vecViewAngles;    // Command view angles (local space)
	Vector m_vecAbsViewAngles;    // Command view angles (world space)
	int m_nButtons;            // Attack buttons.
	int m_nOldButtons;        // From host_client->oldbuttons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector m_vecVelocity;        // edict::velocity        // Current movement direction.
	Vector m_vecOldVelocity;
	float somefloat;
	Vector m_vecAngles;        // edict::angles
	Vector m_vecOldAngles;

	// Output only
	float m_outStepHeight;    // how much you climbed this move
	Vector m_outWishVel;        // This is where you tried 
	Vector m_outJumpVel;        // This is your jump velocity
								 // Movement constraints    (radius 0 means no constraint)
	Vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	bool m_bConstraintPastRadius;        ///< If no, do no constraining past Radius.  If yes, cap them to SpeedFactor past radius

	void SetAbsOrigin(const Vector &vec);
	const Vector &GetAbsOrigin() const;

private:
	Vector m_vecAbsOrigin;        // edict::origin
};

class CMoveHelper
{
public:
	void SetHost(void *host)
	{
		typedef void(__thiscall *o_SetHost)(void*, void*);
		CallVFunction<o_SetHost>(this, 1)(this, host);
	}
};

class CGameMovement
{
public:
	virtual			~CGameMovement(void) {}

	virtual void	ProcessMovement(C_CSPlayer* pPlayer, CMoveData* pMove) = 0;
	virtual void	Reset(void) = 0;
	virtual void	StartTrackPredictionErrors(C_CSPlayer* pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(C_CSPlayer* pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	virtual Vector const& GetPlayerMins(bool ducked) const = 0;
	virtual Vector const& GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const& GetPlayerViewOffset(bool ducked) const = 0;

	virtual bool			IsMovingPlayerStuck(void) const = 0;
	virtual C_BaseEntity*	GetMovingPlayer(void) const = 0;
	virtual void			UnblockPusher(C_CSPlayer* pPlayer, C_CSPlayer* pPusher) = 0;

	virtual void SetupMovementBounds(CMoveData* pMove) = 0;
};

class CPrediction
{
public:

	void SetupMove(void *player, CUserCmd *userCMD, CMoveHelper *helper, void *move)
	{
		typedef void(__thiscall *o_SetupMove)(void*, void*, CUserCmd*, CMoveHelper*, void*);
		CallVFunction<o_SetupMove>(this, 20)(this, player, userCMD, helper, move);
	}

	void FinishMove(void *player, CUserCmd *userCMD, void *move)
	{
		typedef void(__thiscall *o_FinishMove)(void*, void*, CUserCmd*, void*);
		CallVFunction<o_FinishMove>(this, 21)(this, player, userCMD, move);
	}
};
/*
 * deepRL
 */

#ifndef __DEEP_REINFORCEMENT_LEARNER_H_
#define __DEEP_REINFORCEMENT_LEARNER_H_


#include <stdio.h>
#include <stdint.h>
#include <string>

#include "pyTensor.h"


/**
 * Default name of the Python module to load
 */
#define DEFAULT_RL_MODULE "DQN"

/**
 * Default name of the Python function from the user's module
 * which infers the next action from the current state.
 * The expected function is of the form `def next_action(state):`
 * where state is a pyTorch tensor containing the environment,
 * and the function returns the predicted action.
 */
#define DEFAULT_NEXT_ACTION "next_action"

/**
 * Default name of the Python function from the user's module
 * which recieves rewards and performs training.
 * The expected reward function is of the form 
 * `def next_reward(state, reward, new_episode):`, where the
 * function returns the predicted action and accepts the reward.
 */
#define DEFAULT_NEXT_REWARD "next_reward"


/**
 * Base class for deep reinforcement learning,
 * using Python & pyTorch underneath with C FFI.
 */
class deepRL
{
public:
	/**
	 * Create a new instance of a module.
	 */
	static deepRL* Create( uint32_t numInputs, uint32_t numActions, 
					   const char* module=DEFAULT_RL_MODULE,
					   const char* nextAction=DEFAULT_NEXT_ACTION, 
					   const char* nextReward=DEFAULT_NEXT_REWARD );

	/**
	 * Create a new instance of a module.
	 */
	static deepRL* Create( uint32_t width, uint32_t height, 
					   uint32_t channels, uint32_t numActions, 
					   const char* module=DEFAULT_RL_MODULE,
					   const char* nextAction=DEFAULT_NEXT_ACTION, 
					   const char* nextReward=DEFAULT_NEXT_REWARD );

	/**
	 * Destructor
	 */
	virtual ~deepRL();

	/**
	 * Globally load Python scripting interpreter.
	 * LoadInterpreter is automatically called before tensors or scripts are run.
	 * It can optionally be called by the user at the beginning of their program to
	 * load Python at that time. It has internal protections to only be called once.
	 */
	static bool LoadInterpreter();

	/**
	 * From the input state, predict the next action (inference)
	 * This function isn't used during training, for that see NextReward()
	 */
	virtual bool NextAction( Tensor* state, int* action );

	/**
	 * Next action with reward (training)
	 */
	virtual bool NextReward( float reward, bool end_episode );

	/**
	 * End the current episode and start the next episode
	 */
	//virtual void NextEpisode();

	/**
	 * Load script
	 */
	bool LoadModule( const char* module );

	/**
	 * Load script (with arguments)
	 */
	bool LoadModule( const char* module, int argc, char** argv );


protected:
	deepRL();
	
	//bool initLua();
	bool initNetwork( uint32_t numInputs, uint32_t numActions );

#ifdef USE_LUA
	lua_State* L;		/**< Lua/Torch7 operating environment */
	THCState*  THC;	/**< cutorch state */
#endif

	//bool mNewEpisode;

	uint32_t mInputWidth;
	uint32_t mInputHeight;
	uint32_t mNumInputs;
	uint32_t mNumActions;
	
	Tensor* mRewardTensor;
	Tensor* mActionTensor;

	enum
	{
		ACTION_FUNCTION = 0,
		REWARD_FUNCTION,
		NUM_FUNCTIONS
	};

	std::string mModuleName;
	void*	  mModuleObj;
	void* 	  mFunction[NUM_FUNCTIONS];
	void*	  mFunctionArgs[NUM_FUNCTIONS];
	std::string mFunctionName[NUM_FUNCTIONS];

	static bool scriptingLoaded;
};


#endif
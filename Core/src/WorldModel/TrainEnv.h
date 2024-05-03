#include <atomic>
#include <singleton.hpp>
class RLEnvironment{
public:
	RLEnvironment() = default;
	void setDone(bool done) { _done = done; }
	bool done() const { return _done; }
	void setTrain(bool train) { _train = train; }
	bool train() const { return _train; }
    void setReward(float reward) { _reward = reward; }
    float reward() const { return _reward; }
	// RL interface
    void placeRobot(int num, double x, double y, double dir);
    void placeBall(double x, double y, double vx, double vy);
private:
	std::atomic<bool> _done = false;
	std::atomic<bool> _train = false;
	std::atomic<float> _reward = 0.0f;
};
typedef Singleton<RLEnvironment> TrainEnv;
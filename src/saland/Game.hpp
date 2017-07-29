#ifndef GAME_HPP
#define GAME_HPP

#include <memory>

#include "../sago/GameStateInterface.hpp"

class Game : public sago::GameStateInterface {
public:
	Game();
	virtual bool IsActive() override;
	virtual void Draw(SDL_Renderer* target) override;
	virtual void ProcessInput(const SDL_Event& event, bool &processed) override;
	virtual void Update() override;
	virtual ~Game();
private:
	struct GameImpl;
	std::unique_ptr<GameImpl> data;
};

#endif /* GAME_HPP */


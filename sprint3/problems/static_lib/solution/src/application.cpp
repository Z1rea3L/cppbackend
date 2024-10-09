#include "application.h"

#include <cassert>
#include <chrono>
#include "logger.h"

Application::Application(boost::asio::io_context &ioc, std::filesystem::path config, std::filesystem::path dir_to_content)
    : m_ioc(ioc),
      dir_to_content_(dir_to_content),
      strand(boost::asio::make_strand(ioc)),
      m_manual_ticker(true) {
  if (!(std::filesystem::exists(config) && std::filesystem::exists(dir_to_content_))) {
    throw std::logic_error("Wrong path, config="s + config.string() + ", content="s + dir_to_content.string());  //? maybe need more output information
  }
  m_game = json_loader::LoadGame(config);
}

Application::~Application() {
  // todo
  // close resource
}

void Application::SetManualTicker(bool enable) {
  m_manual_ticker = enable;
}

bool Application::IsManualTicker() const {
  return m_manual_ticker;
}

void Application::SetRandomSpawn(bool enable){
  m_game.SetRandomSpawn(enable);
}


const std::filesystem::path &Application::GetContentDir() const noexcept {
  return dir_to_content_;
}

std::optional<std::reference_wrapper<const model::Map>> Application::FindMap(const model::Map::Id &id) const noexcept {
  return m_game.FindMap(id);
}

const model::Game::Maps &Application::GetMaps() const noexcept {
  return m_game.GetMaps();
}

model::Player &Application::JoinGame(model::Map::Id id, const std::string &user_name) {
  //  get if exist or get created session
  auto sess = m_game.GetSession(id);
  if (!sess) {
    throw std::runtime_error("Fail to open or create ssesion");
  }

  auto &player = m_player_list.CreatePlayer(user_name);
  player.SetSession(sess);
  sess->AddDog(player.GetDog());

  return player;
}

model::Player *Application::FindPlayer(const model::Token &t) {
  return m_player_list.FindPlayer(t);
}

const model::PlayerList::Container &Application::GetPlayers() const noexcept {
  return m_player_list.GetContainer();
}

void Application::Update(std::chrono::milliseconds ms) {
  if(!m_manual_ticker && ms.count() > 30000ul){   // fail if more than 30s
    throw std::runtime_error("Time for aplication update state is very long");
  }

  double delta = std::chrono::duration<double>(ms).count();
  m_game.Update(delta);
}

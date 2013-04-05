#include <iostream>

#include <pipeline/playerfactory.hpp>
#include <pipeline/player.hpp>


int main(int argc, char **argv)
{
  mediapipeline::PlayerFactory::bsp_t pf1 = mediapipeline::PlayerFactory::getFactory();
  mediapipeline::PlayerFactory::bsp_t pf2 = mediapipeline::PlayerFactory::getFactory();

  std::cout << "are they same ptr? : " << (pf1.get() == pf2.get() ? "yes" : "no" ) << std::endl;

  mediapipeline::Player::bsp_t player = pf1->create("transport-type");
//  mediapipeline::Player::bsp_t player = pf1->create("transport-type", "url", "1", "2");

  player->load(); // pipeline created. 

  player->play(); // pipeline->play()

  player->unload(); // pipeline->unload()

  return 0;
}

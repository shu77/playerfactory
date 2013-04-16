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

  //player->load(); // pipeline created. 
  //player->play(); // pipeline->play()
  //player->unload(); // pipeline->unload()
  
  int selectnum = 0x00;
  do
  {
      std::cout << "============================================================= \n";
      std::cout << " WebOs - mediapipeline CLI(command line interface) debug menu \n";
      std::cout << "============================================================= \n";
      std::cout << " 11. load (playbin)\n";
      std::cout << " 12. play \n";
      std::cout << " 18. unload \n";
	  std::cout << " 19. get duration \n";
      std::cout << "============================================================= \n";
      std::cout << " 21. load (stream)\n";
	  std::cout << "============================================================= \n";
      std::cout << " 99. Exit \n";
      std::cout << "============================================================= \n";

	  std::cout << "Select List Function integer:";
      std::cin >> selectnum;

	  switch(selectnum)
	  {
            case 11 :
				  {
				  	  MEDIA_CLIPOPT_T clipOpt;
				      player->load(&clipOpt); // pipeline created. 
            	  }
				  break;

            case 12 :
				  player->play();
                  break;

            case 18 :
				  player->unload();
				  break;
		    case 19 :
				  player->duration();
				  break;

            case 21 :
				  {
				  	  MEDIA_STREAMOPT_T streamOpt;
					  MEDIA_FORMAT_T mediaFormatType;
				      player->load(&streamOpt, mediaFormatType); // pipeline created. 
            	  }
				  break;

            case 99 :
            default :
                break;
	  }
	  std::cin.clear();
	  //std::cout << "selectnum = " << selectnum << "\n";
    
  }while(selectnum != 99);




  return 0;
}

#include <iostream>

#include <pipeline/playerfactory.hpp>
#include <pipeline/player.hpp>
#include <string>
#include <string.h>
#include <pipeline/utils/options.hpp>


using namespace std;
using namespace
    mediapipeline::utils;
#if 0
typedef enum
{
  GST_LEVEL_NONE = 0,
  GST_LEVEL_ERROR = 1,
  GST_LEVEL_WARNING = 2,
  GST_LEVEL_FIXME = 3,
  GST_LEVEL_INFO = 4,
  GST_LEVEL_DEBUG = 5,
  GST_LEVEL_LOG = 6,
  GST_LEVEL_TRACE = 7,
  /* add more */
  GST_LEVEL_MEMDUMP = 9,
  /* add more */
  GST_LEVEL_COUNT
} GstDebugLevel;
#endif

static void *
DEBUG_thread (void *ptr)
{ /* make main loop for gstreamer bus callback */
  GMainLoop **loop = (GMainLoop **) ptr;
  *loop = g_main_loop_new (NULL, FALSE);
  std::cout << "start gstreamer main loop run.";
  g_main_loop_run (*loop);
}

static void
_DEBUG_thread_quit (void *ptr)
{ /* delete main loop */
  GMainLoop *loop = (GMainLoop *) ptr;
  g_main_loop_quit (loop);
}

int
main (int argc, char **argv)
{
  mediapipeline::PlayerFactory::bsp_t pf1 =
    mediapipeline::PlayerFactory::getFactory ();
  mediapipeline::PlayerFactory::bsp_t pf2 =
    mediapipeline::PlayerFactory::getFactory ();

  pthread_t serviceThread;
  pthread_attr_t threadAttr;
  int result;
  int selectnum = 0x00;
  gboolean pipeCreated = false;

  std::cout << "are they same ptr? : " << (pf1.get () ==
            pf2.get ()? "yes" : "no") << std::endl;
  GMainLoop *gstMainLoop = NULL;

  pthread_attr_init (&threadAttr);
  if ((result =
         pthread_attr_setdetachstate (&threadAttr, PTHREAD_CREATE_DETACHED))) {
    std::cout << "Failed to detach thread, ret =" << result;
  }

  if (0 != (result =
              pthread_create (&serviceThread, &threadAttr, DEBUG_thread,
                              (void *) (&gstMainLoop)))) {
    std::cout << "Thread creation failed, ret =" << result;
    //ret = false;
  }
  mediapipeline::Player::bsp_t player;
  //mediapipeline::Player::bsp_t player = pf1->create ("transport-type");
  //  mediapipeline::Player::bsp_t player = pf1->create("transport-type", "url", "1", "2");
  //player->setGstreamerDebugLevel(0, 0, GST_LEVEL_TRACE);

  do {
    std::cout <<
              "============================================================= \n";
    std::cout <<
              " WebOs - mediapipeline CLI(command line interface) debug menu \n";
    std::cout <<
              "============================================================= \n";
    std::cout << " 11. load (playbin)\n";
    std::cout << " 12. play \n";
    std::cout << " 13. pause \n";
    std::cout << " 17. setPlaybackRate \n";
    std::cout << " 18. unload \n";
    std::cout << " 19. get duration \n";
    std::cout <<
              "============================================================= \n";
    std::cout << " 21. load (stream)\n";
    std::cout <<
              "============================================================= \n";
    std::cout << " 99. Exit \n";
    std::cout <<
              "============================================================= \n";
    std::cin.clear ();          //clear badbit flag
    std::cin.sync ();           //clear stdin stream

    std::cout << "Select List Function integer:";
    std::cin >> selectnum;

    std::cin.clear ();          //clear badbit flag
    std::cin.sync ();           //clear stdin stream
    std::cin.ignore (INT_MAX, '\n');
    switch (selectnum) {
    case 11:
    {
      if(pipeCreated == false)
        player = pf1->create ("transport-type");
      //MEDIA_CLIPOPT_T clipOpt;
      std::string mystr;
      std::string optionstr;
      mystr.clear ();
      optionstr.clear ();
      std::cout << "Input play URI:";
      getline (std::cin, mystr);
      std::cout << endl;
      optionstr+= " { \"clipOpt\": { ";
      optionstr+= "\"mediafile\":"; // clipoption -> mediafile
      optionstr+= "\"";
      optionstr+= mystr; // value
      optionstr+= "\"";
      optionstr+= ", ";
      optionstr+= "\"level\":";
      optionstr+= "\"4207846\"";
      //optionstr+= " } } ";
      optionstr+= " }  ";

      optionstr+= " , \"wmainfo\": { ";
      optionstr+= " \"wmaVer\": "; // clipoption -> mediafile
      optionstr+= "\"";
      optionstr+= "123456789";
      optionstr+= "\"";
      optionstr+= " } } ";

      std::cout << " -- boost json string : " << optionstr << endl;
#if 0 //API test locally.
      mediapipeline::utils::Options::bsp_t op (new mediapipeline::utils::Options ());
      // format sample..
      //op->loadJSON(" { \"clipOpt\": { \"filename\": \"test.txt\", \"level\": \"4207846\" } }");
      op->loadJSON((const std::string) optionstr);
      cout << "clipOpt.mediafile : " << op->getString ("clipOpt.mediafile") << endl;
      cout << "clipOpt.level : " << op->getInt ("clipOpt.level") << endl;
      cout << "wmainfo.wmaVer : " << op->getString ("wmainfo.wmaVer") << endl;
#endif
      player->load ((const std::string)optionstr);        // pipeline created.
      optionstr.clear();
    }
    break;

    case 12:
      player->play ();
      break;
    case 13:
      player->pause ();
      break;
    case 17:
    {
      std::string num;
      gfloat temp;
      num.clear ();
      std::cout << "Input playback rate (0.x):";
      getline (std::cin, num);

      temp = std::atof (num.c_str ());
      std::cout << "rate inputed : " << temp << endl;
      player->setPlaybackRate (temp); //(reinterpret_cast<float>temp);
      break;
    }
    case 18:
      player->unload ();
      break;
    case 19:
      player->duration ();
      break;

    case 21:
    {
      if(pipeCreated == false)
        player = pf1->create (MEDIA_TRANS_BUFFERSTREAM);
      
      //MEDIA_STREAMOPT_T streamOpt;
      std::string optionstr;

      player->load (optionstr);      // pipeline created.
    }
    break;

    case 99:
    default:
      break;
    }
  } while (selectnum != 99);


  _DEBUG_thread_quit ((void *) (gstMainLoop));

  return 0;
}

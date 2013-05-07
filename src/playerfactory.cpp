/*
 * playerfactory.cpp
 *
 *  Created on: 2013. 4. 3.
 *      Author: jeongseok.kim
 */

#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif
#include <pipeline/playerfactory.hpp>
#include <pipeline/generic/genericplayer.hpp>
#include <pipeline/custom/customplayer.hpp>
#include <pipeline/generic/dlnaplayer.hpp>

using namespace std;
using namespace mediapipeline;

boost::weak_ptr < PlayerFactory > PlayerFactory::_internal_ptr;

PlayerFactory::PlayerFactory ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("PlayerFactory");
  LOG_D ("created", "PlayerFactory");
  GError *pErr = NULL;
  /* Initialize GStreamer */
  if (!g_thread_supported ()) {
    g_thread_init (NULL);
  }

  if (!gst_init_check (NULL, NULL, &pErr)) {
    LOG_D ("Error:" "%s\n", pErr->message);
    g_error_free (pErr);
    //return ERROR; //TODO: check fail case.
  }
}

PlayerFactory::~PlayerFactory ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("PlayerFactory");
  LOG_D ("removed", "PlayerFactory");
}

const string
PlayerFactory::getName ()
{
  return "mediapipeline::PlayerFactory";
}

PlayerFactory::bsp_t PlayerFactory::getFactory ()
{

  PlayerFactory::bsp_t instance = _internal_ptr.lock ();

  LOG_FUNCTION_SCOPE_NORMAL_D ("PlayerFactory");

  if (!instance) {
    LOG_D ("an instance will be created.", "PlayerFactory");
    instance.reset (new PlayerFactory ());
    _internal_ptr = instance;
  }
  return instance;
}


//TODO : 기존 lmf_api에서 uri check로 분기타던 FCC 및 MCAST 는 app 단에서 URI 확인 및 transport type 지정 필요.
Player::bsp_t PlayerFactory::create (const std::string transport_type)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("PlayerFactory");

  // need convert.. (string to local enum)

  return this->create (1);
}

Player::bsp_t PlayerFactory::create (unsigned int transport_type)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("PlayerFactory");
  Player::bsp_t player;

  // 1. check transport type.
  if (transport_type >= MEDIA_TRANS_PLAYBIN_START
      && transport_type <= MEDIA_TRANS_PLAYBIN_START) {
    cout << "create GenericPlayer group" << endl;

    switch(transport_type){
      case MEDIA_TRANS_DLNA:
        player.reset (new DlnaPlayer ());
        break;
      default :
        player.reset (new GenericPlayer ());
        break;
    }
  }
  else if(transport_type >= MEDIA_TRANS_STATIC_START
        && transport_type <= MEDIA_TRANS_STATIC_END) {
    cout << "create CustomPlayer" << endl;
    player.reset (new CustomPlayer ());
  }
  else{

  }

  return player;
}

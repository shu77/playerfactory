#include <iostream>

#include <pipeline/utils/options.hpp>


using namespace std;


int
main (int argc, char **argv)
{
  mediapipeline::utils::Options::bsp_t op (new mediapipeline::utils::
      Options ());

  op->loadJSON
      (" { \"debug\": { \"filename\": \"test.txt\", \"level\": \"4207846\" } }");

  cout << "debug.filename : " << op->getString ("debug.filename") << endl;

  cout << "debug.level : " << op->getInt ("debug.level") << endl;

  return 0;
}

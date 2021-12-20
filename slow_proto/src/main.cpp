#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

class Widget {
  public:
    int blah = 0;
  Widget() {}
  Widget(const Widget& thing) {
    printf("copy constructor\n");
    blah = thing.blah;
  }
  void operator=(const Widget& thing) {
    printf("copy assign operator\n");
    blah = thing.blah;
  }
  Widget(Widget&& thing) {
    printf("move constructor2\n");
    blah = thing.blah;
  }
  void operator=(Widget&& thing) {
    printf("move assignment\n");
    blah = thing.blah;
  }
};

Widget g_w;
const Widget& create() {
  return g_w;
}

int main(void)
{
  //Widget& a = w;
  std::optional<Widget> x;
  x->blah = 6;
  std::optional<Widget> y = *x; // Move or copy?
  printf("inside y: %d\n", y->blah);
  return 0;
}

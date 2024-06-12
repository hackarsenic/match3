#include <memory>
#include "GameController.h"

int main()
{
    std::unique_ptr<GameController> gc = std::make_unique<GameController>();
    gc->Execute();

    return 0;
}

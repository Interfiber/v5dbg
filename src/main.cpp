#include "main.h"
#include "v5dbg/debug.h"

// Global debug server state
v5dbg_server_state_t sState{};

void
autonomous(void)
{
  $ntask $function
}

void
initialize(void)
{
  // Start debug server

  sState = V5Dbg_AllocateServerState();
  V5Dbg_StartServer(&sState);
}

void
disabled(void)
{
}

void
competition_initialize(void)
{
}

void
opcontrol(void)
{
  $ntask;
  $function;

  int x = 0;
  $expose(x);

  while (true)
  {
    $cbreak(x % 4 == 0 && x != 0);

    printf("%i\n", x);

    x++;
    pros::delay(300);
  }
}

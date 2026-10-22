#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#include "BudgetManager.h"

// Starts the local web server. The browser sends requests to this class.
void runServer(BudgetManager& manager, int port);

#endif

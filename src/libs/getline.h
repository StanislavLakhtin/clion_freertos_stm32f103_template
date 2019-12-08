//
// Created by Stanislav Lakhtin on 08/11/2019.
//

#include <libopencm3/cm3/common.h>

#ifndef BBB_GETLINE_H
#define BBB_GETLINE_H

BEGIN_DECLS

int getline( char * buf, unsigned bufsiz, int (* get)( void ), void (* put)( char ch ));

END_DECLS

#endif //BBB_GETLINE_H

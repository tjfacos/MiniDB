//
// Created by root on 7/28/25.
//

#ifndef POINTER_H
#define POINTER_H
#include <cstdint>


class Pointer {

    uint16_t file       ;
    uint16_t partition  ;
    uint16_t page       ;
    uint16_t slot       ;

public:

    Pointer(uint16_t file, uint16_t partition, uint16_t page, uint16_t slot);

    uint16_t getFile()  const;
    uint16_t getPart()  const;
    uint16_t getPage()  const;
    uint16_t getSlot()  const;


};



#endif //POINTER_H

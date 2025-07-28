//
// Created by root on 7/28/25.
//

#ifndef POINTER_H
#define POINTER_H



class Pointer {

    unsigned int file       ;
    unsigned int partition  ;
    unsigned int page       ;
    unsigned int slot       ;

public:

    Pointer(unsigned int file, unsigned int partition, unsigned int page, unsigned int slot);

    unsigned int getFile()  const;
    unsigned int getPart()  const;
    unsigned int getPage()  const;
    unsigned int getSlot()  const;


};



#endif //POINTER_H

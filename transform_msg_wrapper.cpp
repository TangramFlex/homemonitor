#include "code-gen/out/transform/include/my_transform.hpp"
#include <string>
#include <iostream>
#include <cstring>

// We are wrapping this in an extern "C" so that the transform function
// is callable as C code.
extern "C" {
int transform(void *msg1, void *msg2);
void *create_and_populate_input_msg(Old::Motion::EnumItem livingRoomMotion1, \
                                                                   Old::Motion::EnumItem livingRoomMotion2, \
                                                                   Old::Entry::EnumItem garageDoorEntry, \
                                                                   const char* frontDoorEntry, \
                                                                   int32_t basementMotion1, \
                                                                   int32_t patioTemp);
const char* get_living_room_motion_1_output_string(void *msg);
const char* get_living_room_motion_2_output_string(void *msg);
const char* get_garage_door_entry_output_string(void *msg);
const char* get_front_door_entry_output_string(void *msg);
const char* get_basement_motion_1_output_string(void *msg);
const char* get_patio_temp_output_string(void *msg);
void free_input_msg(void *msg) ;
void *create_output_msg();
void free_output_msg(void *msg);

void *create_and_populate_input_msg(Old::Motion::EnumItem livingRoomMotion1, \
                                                                   Old::Motion::EnumItem livingRoomMotion2, \
                                                                   Old::Entry::EnumItem garageDoorEntry, \
                                                                   const char* frontDoorEntry, \
                                                                   int32_t basementMotion1, \
                                                                   int32_t patioTemp) {
    Old::OldSystemState *inputMsg = new Old::OldSystemState;
    inputMsg->setLivingRoomMotion1(livingRoomMotion1);
    inputMsg->setLivingRoomMotion2(livingRoomMotion2);
    inputMsg->setFrontDoorEntry(frontDoorEntry);
    inputMsg->setGarageDoorEntry(garageDoorEntry);
    inputMsg->setPatioTemp(patioTemp);
    inputMsg->setBasementMotion1(basementMotion1);
    return inputMsg;
}

void *create_output_msg() {
    New::NewSystemState *outputMsg = new New::NewSystemState;
    return outputMsg;
}

int transform(void *msg1, void *msg2) {
    // The transform function requires messages to be wrapped in a
    // std::shared_ptr, which is a reference counted wrapper that will, by
    // default, delete the resource when it goes out of scope. We don't want
    // that here since the message is owned by the caller so we supply custom
    // "deleter" C++ lambdas that do nothing
    std::shared_ptr<Old::OldSystemState> spInputMsg((Old::OldSystemState*) msg1, [](void *){});
    std::shared_ptr<New::NewSystemState> spOutputMsg((New::NewSystemState *) msg2, [](void *){});  

    try {
        my_transform::execute(*spInputMsg, *spOutputMsg);
        // std::cout << "After my_transform::execute()" << std::endl;
    } catch (const std::exception &exc) {
        std::cout << "Some exception was thrown;in catch" << std::endl;
        std::cerr << exc.what();
        return -1;
    }

    return 0;
}

const char* get_living_room_motion_1_output_string(void *msg) {
    New::NewSystemState *outputMsg = (New::NewSystemState *) msg;
    std::string livingRoomMotion1 = (*outputMsg).getLivingRoomMotion1();
    // std::cout << "value for livingRoomMotion1:" << livingRoomMotion1 << std::endl;
    return livingRoomMotion1.c_str();
}

const char* get_living_room_motion_2_output_string(void *msg) {
    New::NewSystemState *outputMsg = (New::NewSystemState *) msg;
    std::string livingRoomMotion2 = (*outputMsg).getLivingRoomMotion2();
    // std::cout << "value for livingRoomMotion2:" << livingRoomMotion2 << std::endl;
    return livingRoomMotion2.c_str();
}

const char* get_garage_door_entry_output_string(void *msg) {
    New::NewSystemState *outputMsg = (New::NewSystemState *) msg;
    std::string garageDoorEntry = (*outputMsg).getGarageDoorEntry();
    // std::cout << "value for garageDoorEntry:" << garageDoorEntry << std::endl;
    return garageDoorEntry.c_str();
}

const char* get_front_door_entry_output_string(void *msg) {
    New::NewSystemState *outputMsg = (New::NewSystemState *) msg;
    std::string frontDoorEntry = (*outputMsg).getFrontDoorEntry();
    // std::cout << "value for frontDoorEntry:" << frontDoorEntry << std::endl;
    return frontDoorEntry.c_str();
}

const char* get_basement_motion_1_output_string(void *msg) {
    New::NewSystemState *outputMsg = (New::NewSystemState *) msg;
    std::string basementMotion1 = (*outputMsg).getBasementMotion1();
    // std::cout << "value for basementMotion1:" << basementMotion1 << std::endl;
    return basementMotion1.c_str();
}

const char* get_patio_temp_output_string(void *msg) {
    New::NewSystemState *outputMsg = (New::NewSystemState *) msg;
    int32_t patioTemp = (*outputMsg).getPatioTemp();
    // std::cout << "value for patioTemp:" << patioTemp << std::endl;
    // forgive me if this is dumb, I am sure there is a better way
    char str[100];
    sprintf(str, "%d", patioTemp);
    return std::string(str).c_str();
}

void free_input_msg(void *msg) {
    Old::OldSystemState *inputMsg =
        (Old::OldSystemState *) msg;
    delete inputMsg;
}

void free_output_msg(void *msg) {
    New::NewSystemState *outputMsg =
        (New::NewSystemState *) msg;
    delete outputMsg;
}
}
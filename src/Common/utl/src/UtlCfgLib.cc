/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "UtlCfgLib.h"

bool UtlCfgLib::isCondJump(Instruction *i) { 
    if(!i->HasAttribute(InstructionIsConditionalAttributeName))
        return false;
    string conditional = ((SerialisableStringAttribute&)i->GetAttribute(InstructionIsConditionalAttributeName)).GetValue();
    return conditional == "true" && isJump(i);
}
bool UtlCfgLib::isLoad(Instruction *i) { 
    if(!i->HasAttribute(InstructionTypeAttributeName))
        return false;
    string type = ((SerialisableStringAttribute&)i->GetAttribute(InstructionTypeAttributeName)).GetValue();
    return type == InstructionTypeLoad;
}
bool UtlCfgLib::isStore(Instruction *i) { 
    if(!i->HasAttribute(InstructionTypeAttributeName))
        return false;
    string type = ((SerialisableStringAttribute&)i->GetAttribute(InstructionTypeAttributeName)).GetValue();
    return type == InstructionTypeStore;
}

bool UtlCfgLib::isForwardJump(Instruction *i) { 
    if(!i->HasAttribute(InstructionTypeAttributeName))
        return false;
    string type = ((SerialisableStringAttribute&)i->GetAttribute(InstructionTypeAttributeName)).GetValue();
    return type == InstructionTypeFwJump;
}
bool UtlCfgLib::isBackwardJump(Instruction *i) { 
    if(!i->HasAttribute(InstructionTypeAttributeName))
        return false;
    string type = ((SerialisableStringAttribute&)i->GetAttribute(InstructionTypeAttributeName)).GetValue();
    return type == InstructionTypeBwJump;
}
bool UtlCfgLib::isJump(Instruction *i) {
    return isForwardJump(i) || isBackwardJump(i);
}
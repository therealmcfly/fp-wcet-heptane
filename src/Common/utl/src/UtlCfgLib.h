/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UtlCfgLib.h
 * Author: brouxel
 *
 * Created on 26 avril 2018, 13:50
 */

#ifndef UTLCFGLIB_H
#define UTLCFGLIB_H

#include "GlobalAttributes.h"
#include "Instruction.h"
using namespace cfglib;

namespace UtlCfgLib {
    string instr_type_string_from_type(const string &type);
  
    string instr_type_type_from_string(const std::string &type);
    bool isCondJump(cfglib::Instruction *i);
    bool isLoad(cfglib::Instruction *i);
    bool isStore(cfglib::Instruction *i);
    
    bool isForwardJump(cfglib::Instruction *i);
    bool isBackwardJump(cfglib::Instruction *i);
    bool isJump(cfglib::Instruction *i);
};

#endif /* UTLCFGLIB_H */


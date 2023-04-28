#version 460 core

out int outEntID;

uniform int entID;

void main() {
    outEntID = entID;
}
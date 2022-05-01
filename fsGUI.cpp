//#include "main.h"
//
//void menu()
//{
//    static bool textEnable = 1;
//    ImGui::Checkbox("text", &textEnable);
//    ImGui::Spacing();
//    if (textEnable)
//    {
//        static char text[1020] =
//            "/*\n"
//            " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
//            " the hexadecimal encoding of one offending instruction,\n"
//            " more formally, the invalid operand with locked CMPXCHG8B\n"
//            " instruction bug, is a design flaw in the majority of\n"
//            " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
//            " processors (all in the P5 microarchitecture).\n"
//            "*/\n\n"
//            "label:\n"
//            "\tlock cmpxchg8b eax\n";
//
//        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
//
//        ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
//    }
//}
//

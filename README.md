[//]: # (GitHub doesn't seem to support style="text-align: center")
<h1 align="center">leprac</h1>

<p align="center">
  <img alt="GitHub License" src="https://img.shields.io/github/license/BAN-43-32532/leprac?style=plastic">
  <img alt="GitHub top language" src="https://img.shields.io/github/languages/top/BAN-43-32532/leprac?style=plastic">
  <img alt="GitHub Actions Workflow Status" src="https://img.shields.io/github/actions/workflow/status/BAN-43-32532/leprac/cmake-single-platform.yml?style=plastic">
  <img alt="GitHub Release" src="https://img.shields.io/github/v/release/BAN-43-32532/leprac?style=plastic">
</p>

A practice tool for [Len'en Project](https://lenen.wiki.gg/) shmups.

[//]: # (### Custom ImGui Style)

[//]: # ()
[//]: # (You may not be satisfied for ImGui integrated styles &#40;Dark, Light and Classic&#41; and want to write your own. There are)

[//]: # (also plenty of styles shared in the community &#40;)

[//]: # (see [Post your color styles/themes here #707]&#40;https://github.com/ocornut/imgui/issues/707&#41;&#41;.)

[//]: # ()
[//]: # (However, ImGui refuses to provide a configure format for custom style &#40;)

[//]: # (from [How to load/save imgui styles #101]&#40;https://github.com/ocornut/imgui/issues/101&#41;)

[//]: # (to [ImGuiStyle Struct Save and Load functions #8762]&#40;https://github.com/ocornut/imgui/pull/8762&#41;&#41;, which means you have)

[//]: # (to write codes instead of simply loading a file. leprac provides two ways to use your custom style.)

[//]: # ()
[//]: # (1. Provide a valid cpp source code which modifies `ImGui::GetStyle&#40;&#41;`, and set the path in Setting. You may)

[//]: # (   take [this comment]&#40;https://github.com/ocornut/imgui/issues/707#issuecomment-252413954&#41; for example. Typically, this)

[//]: # (   involves assigning values to the `ImGuiStyle` struct `ImGui::GetStyle&#40;&#41;` and the `ImVec4[]` array)

[//]: # (   `ImGui::GetStyle&#40;&#41;.Colors`. leprac will parse these cpp codes in a trivial way. It should work as long as you write)

[//]: # (   normally.)

[//]: # (2. Put your source code in `asset/style/user_custom_style.hpp` and build leprac executable yourself &#40;see **Build**&#41;.)

[//]: # ()
[//]: # (Note: If you set a custom style, the style option will work as a fallback if you don't explicitly assign some values.)

### Reference & Special Thanks

- [thprac](https://github.com/touhouworldcup/thprac) - _originally by Ack_

- [Game Tools and Modifications](https://lenen.wiki.gg/wiki/Game_Tools_and_Modifications) at Len'en Wiki
  - Game Enhancement Patcher - _by [Popfan](https://lenen.wiki.gg/wiki/User:Popfan)_
  - Absurdly Extra Patch for RMI - _by Necrotek_
  - Gameskip Patch for BPoHC - _by Mona Lisa's Hands, TwoEyedMike_
  - Le01prac - _by [Priw8](https://link.priw8.com/)_
  - EMS_RMIprac - _by [Oscar](https://www.youtube.com/@oscar1_41_1)_
  - Score Patch for BPoHC - _by [RosePenguin100](https://www.youtube.com/@thetacticianmusician6565)_
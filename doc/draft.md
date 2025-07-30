todo:
- key display
- locale emulator?
- window resize?

1. Basic SDL3 (I wonder if it's still needed since gui injection needs ImGui with DX11 backend, besides I don't like sdl.dll which can be embedded in leprac.exe, though) and ImGui init. Load default en, zh, ja fonts and save whether they are missing. When the user is choosing language, warn them if font is missing. 
   1. I do think you can enable font customization by either providing a font folder that encourages users to put ttf/ttc there, or providing `path_fonts = ["font_path1", "font_path2", etc]` there. However I don't think it needed currently, until I finally make a universal practool that supports many Len'en and other indies. 
   2. Load `config.toml` (class `Config`) and asset (class `Asset`)
      ```
      # config.toml
      
      language = "en" / "zh" / "ja"
      path_le01 = a string of absolute or relative path to executable "Le01.exe"
      # path_le02 ~ path_le04, similarly
      debug_mode = true / false
      # path_fonts
      ```
      Note: the order is not mandatory, but whenever some value is autofilled or something else, reorder them as above
   3. class `Asset` read all asset toml files by `b::embed<"asset/foo/bar.toml">()` once and save them as `std::string` (idk if I further need the original file format). class `Literal` parses `literal.toml` and class `Asset` itself parses address, frame etc. I'm still wondering either classes that require literal display storing literals themselves loaded from class Literal once and forever, or they call `Literal` whenever needed. I will not use i18n external support.
   4. If `config.toml` is not found, create it with only `debug_mode = false` set
   5. If language is not found, pop up an imgui window asking which language to choose (hint literal loaded from it or I think maybe hard-encoded is also okay)
   6. class `Config` will decide which language to display in gui, which will pass to class Literal
      Note: class `Game` will check the language patch of the game which is independent of this display language. The patch variable is reserved for some future use
2. Check if there are process "Le01.exe" ~ "Le04.exe" running
   1. If there are exactly one process running
      1. Pass the game enum to class Game
      2. Pop up an imgui window saying that the game is detected and ask if the user agree to attach to it.
      3. If agreed, do the attachment. If success, inform the user and hint that you can press `F12` in game to conduct further settings, just as thprac. I feel it may also be acceptable to explore more ways for modification control, or you can operate at the outside window of leprac. Some other hint: when the game is closed, reopen the leprac main window?
      4. If disagreed or attach fail, warn and open the main leprac window as usual. 
   2. If there are plural processes running, pop up an imgui window asking which game to attach to, or no attachment.
   3. If there are no process running, open main window as usual. 
3. Main window description:
   1. This is an imgui window. 
   2. Section Setting has some options (I copied thprac with minor change, just for reference):
      1. Launch behavior
         1. Checkbox: Launch leprac with admin rights (privilege?)
         2. Combo: When starting leprac, if a game exists under the same directory: 
            1. Launch game
            2. Open launcher
            3. Always ask
         3. Checkbox: Do not search for any ongoing game when launching leprac
         4. Reflective launch (?): Enable if the game behaves abnormally when launched by leprac but not when launched directly. This will nullify any functionality that requires a direct game launch from leprac
         
            Idk what it exactly means; (?): means there is a gray (?) icon by imgui that when you move your mouse upon, a floating window hint appears. 
      2. Launcher
         1. Combo: Theme: Dark / Light / Classic / Custom
            
            Default is Dark. Idk what Custom means
         2. Combo: After launching game: 
            1. Minimize launcher
            2. Close launcher
            3. Do nothing
         3. Checkbox: Auto set default start game (?): When enabled, the first game in the game Combo will be auto used as the default starting game. Manually set default startup games overrides this option
         4. Combo: "Apply leprac" option default state: Previous state / Open / Close
         5. Open data directory (thprac uses Appdata but I think config.toml is enough. So this button may open config.toml with an external default editor)
      3. Game adjustments
         1. Resizable window (make game window resizable. I know ResizeEnable can do but I feel it possible to integrate it in this practool)
      4. Language
      5. Update
         1. Combo: Check update when: opening launcher / executing leprac / Never
         2. Update w/o confirmation (do nothing if previous is set Never)
         3. executable filename after update:
            1. Keep downloaded filename
            2. Use the previous filename
            3. Use "leprac.exe"
         4. Button: Check for update
      6. About
         1. Version info
         2. License info of ego and external
         3. Special thanks (??)
   3. Section Tool has some buttons:
      1. Apply leprac to ongoing game
      2. Random game
      3. Random shottype
   4. Section Game has some cards that lists all possible games (i.e. Le01 - Le04) with game name fetched from class Literal. With each card opened, you can set its path, open custom.exe, open folder etc. 

# Project Structure

- main.cpp:
  - SDL stuff (if needed)
  - log init
  - config init
  - imgui init
  - game init
  - address init
- logger.h
  - init: nothing to do if SDL_Log; open log file o/w
  - one-key switch between SDL_Log and file log
  - support logger << sth and Log::append(format)
  - support hex print for debug
  - log level (Info, Warn, Error, Debug). name conflict?
- config.h
  - aa
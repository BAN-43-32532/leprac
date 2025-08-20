todo:
- key display
- locale emulator?
- window resize?

1. Check if there are process "Le01.exe" ~ "Le04.exe" running
   1. If there are exactly one process running
      1. Pass the game enum to class Game
      2. Pop up an imgui window saying that the game is detected and ask if the user agree to attach to it.
      3. If agreed, do the attachment. If success, inform the user and hint that you can press `F12` in game to conduct further settings, just as thprac. I feel it may also be acceptable to explore more ways for modification control, or you can operate at the outside window of leprac. Some other hint: when the game is closed, reopen the leprac main window?
      4. If disagreed or attach fail, warn and open the main leprac window as usual. 
   2. If there are plural processes running, pop up an imgui window asking which game to attach to, or no attachment.
   3. If there are no process running, open main window as usual. 
2. Main window description:
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
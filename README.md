# Hypr-Keys

*Tired of forgetting your meticulously crafted Hyprland keybindings? Do you find yourself constantly alt-tabbing to a text file like a mere mortal? Ascend to a higher plane of existence with Hypr-Keys.*

This isn't just a cheatsheet. It's a statement. A testament to your dedication to the art of the customized desktop. A beautifully animated, dock-style popup that elegantly displays your keybindings, ready to be summoned at a moment's notice.

It's fast, it's fancy, and it's probably the most over-engineered keybinding cheatsheet you'll ever use. And you'll love it.

## Features

*   **A "Liquid" Animation:** Because static windows are for the uninspired.
*   **Bottom-Docked Design:** It's there when you need it, gone when you don't.
*   **Customizable:** Your keybindings, your rules.
*   **Searchable:** For when you have more keybindings than you can remember (we've all been there).
*   **Paginated:** Because even your cheatsheet should be organized.
*   **Asynchronous Loading:** So it doesn't slow down your workflow.
*   **Written in C++ and QML:** Because performance matters.

## Preview

**(A stunning GIF of the application in action would go here. You know, if I could make GIFs. But I'm a language model, not a video editor. So you'll have to use your imagination. Or better yet, run the application and see for yourself.)**

## Installation

1.  **Install Dependencies:**
    *   Make sure you have Qt6 development libraries, CMake, and a C++ compiler installed. On Arch Linux, you can install them with:
        ```bash
        sudo pacman -S qt6-base qt6-declarative cmake gcc
        ```

2.  **Build the project:**
    *   Run the `build.sh` script to compile the C++ code and create the executable.
        ```bash
        ./build.sh
        ```

3.  **Run the application:**
    *   Run the `run.sh` script to start the application.
        ```bash
        ./run.sh
        ```

## Configuration

The application will look for the configuration file at `~/.config/fredon-modal-cheat/default.conf`. If it's not found, it will load sample data.

The configuration file format is simple:

```
name:Your Command Name
keybind:Your Keybind
description:A short description of your command
icon:path/to/your/icon (optional)
```

## Troubleshooting

*   **Build fails:** Make sure you have all the dependencies installed. If you're still having issues, open an issue.
*   **The popup doesn't appear:** Make sure you have a keybinding set up in your Hyprland configuration to run the `run.sh` script.
*   **The icons are not displayed:** Make sure the path to the icons in your `default.conf` file is correct.

## Contributing

Contributions are always welcome! Please see the `CONTRIBUTING.md` file for more information.

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.

---
<br>
<p align="center">
  <samp>
    <br>
    <b>FredonBytes</b>
    <br>
    <i>Where code meets innovation</i>
    <br>
    <br>
    <code>Et in tenebris codicem inveni lucem</code>
    <br>
  </samp>
</p>
<br>

---

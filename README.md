# UPSeller Dashboard Monitor (ESP32 CYD)

![Project Status](https://youtu.be/HlK_My8J5L8)

A custom-built IoT Dashboard to monitor e-commerce metrics in real-time using an ESP32-2432S028 (Cheap Yellow Display). This project automates data visualization from **UpSeller**, displaying Orders (To Ship, Print, Retrieve), Inventory, and Profit reports directly on a touch screen.

##  Project Goal & Philosophy

This project aims primarily to demonstrate an automation concept I developed to visualize the key data I selected as important for my daily workflow. I designed specific screens in **SquareLine Studio** that fit my needs.

**The main purpose of sharing this repository is to inspire you.**

I want you to see what is possible and adapt it to your own needs. You can:
* Take the code logic and make it work with your own ERP (Bling, Tiny, Shopify, etc.).
* Use the Python backend structure to scrape different data.
* Use the `.stl` case files for a completely different project (like a clock or weather station).

The functions are unlimited! If you have any questions regarding SquareLine Studio or how I created the menus, feel free to reach out. I am willing to help.

##  Hardware Components

* **Microcontroller:** ESP32-2432S028R (CYD - Cheap Yellow Display)
* **Battery:** LiPo 18650 (2500mAh)
* **Housing:** Custom 3D Printed Case

##  3D Case & Assembly

The project includes a custom enclosure designed to be compact and functional.

* **Files:** Located in the `/3d_parts` folder (`case.stl` and `cover.stl`).
* **Mounting Hardware:**
    * To fix the ESP32 in the `case.stl`, I used **M2 Threaded Inserts**.
    * **Inserts Dimensions:** Ø2mm (ID) x 4mm (Length) x 3.5mm (OD).
    * **Screws:** M2 x 7mm (Ø2mm x 7mm). (The screw can have up to 15mm.)

##  Tech Stack

* **Firmware:** C++ (PlatformIO / Arduino Framework) using **LVGL** for the UI.
* **Backend:** Python with **Selenium** & **Flask** (running on a PC/Server to scrape data).
* **UI Design:** SquareLine Studio.

##  Contributing & Contact

If you want to improve this code, feel free to fork this repository and submit a pull request.

**Created by:** Henrique Michel
 **Contact:** henriquegmichel@gmail.com

##  License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

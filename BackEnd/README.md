# Python Backend (Web Scraper)

This folder contains the "brain" of the operation. It is a Python script that uses Selenium to log in to the **UpSeller** ERP, scrape real-time data, and host a local API (Flask) that the ESP32 connects to.

##  Functionality

This code is tailored for users who use **UpSeller** as their ERP. Out of the box, it visualizes:

1.  **Orders:**
    - To Ship
    - To Print
    - To Retrieve
2.  **Financials (Profit):**
    - Today
    - Last 7 Days
    - Last 30 Days
    - This Month
3.  **Inventory:**
    - Specific tracking for "cp1/cp2" SKUs (You should change this to your products).

##  Recommendation for Adaptation

**I strongly recommend adapting the Inventory section or the entire code to fit your specific product.**

The logic used here (Selenium searching for XPaths and CSS Selectors) can be applied to almost any website.

### "I don't know how to code, what now?"
If you are not a programmer, **ask an AI for help!**
* Copy the `app.py` code.
* Paste it into ChatGPT, Claude, or Gemini.
* Ask: *"I have this code that scrapes UpSeller. Help me change it to scrape [For Your ERP] instead. Here is the HTML of the element I want to capture..."*

The AI helped me through this entire process, and it can help you too! ;)

##  Installation

1.  Install Python 3.x
2.  Install dependencies:
    ```bash
    pip install -r requirements.txt
    ```
3.  Run the bot:
    ```bash
    python app.py
    ```

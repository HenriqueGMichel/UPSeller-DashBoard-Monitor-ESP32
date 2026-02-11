from flask import Flask, jsonify
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
import threading
import time

app = Flask(__name__)

STORE_DATA = {
    "to_ship": "0",
    "to_print": "0",
    "to_retrieve": "0",
    "stock_cp1": "0", 
    "stock_cp2": "0",
    "profit_today": "0,00",
    "profit_7d": "0,00",
    "profit_30d": "0,00",
    "profit_month": "0,00",
    "total_ml": 0,
    "total_shopee": 0
}

def force_click(driver, element):
    driver.execute_script("arguments[0].click();", element)

def bot_manager():
    options = webdriver.ChromeOptions()
    options.add_argument("--log-level=3")
    options.add_argument("--start-maximized") 
    
    driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()), options=options)
    wait = WebDriverWait(driver, 10)
    
    driver.get("https://app.upseller.com/pt/login")
    print("Sixty seconds for you Log-In")
    time.sleep(60)
    
    if len(driver.window_handles) > 1:
        driver.switch_to.window(driver.window_handles[-1])

    while True:
        try:
            # --- 1. ORDERS ---
            driver.get("https://app.upseller.com/pt/order/all-orders")
            time.sleep(5) 
            
            tabs = ["Para Enviar", "Para Imprimir", "Para Retirada"]
            sum_ml = 0
            sum_shopee = 0
            
            for tab_name in tabs:
                try:
                    xpath_tab = f"//span[@title='{tab_name}']"
                    elem_tab = driver.find_element(By.XPATH, xpath_tab)
                    
                    try:
                        parent = elem_tab.find_element(By.XPATH, "./../..")
                        badge_num = parent.find_element(By.CSS_SELECTOR, ".item_num").text
                        
                        if tab_name == "Para Enviar": STORE_DATA["to_ship"] = badge_num
                        if tab_name == "Para Imprimir": STORE_DATA["to_print"] = badge_num
                        if tab_name == "Para Retirada": STORE_DATA["to_retrieve"] = badge_num
                    except: pass

                    force_click(driver, elem_tab)
                    time.sleep(4)
                    
                    count_shopee = len(driver.find_elements(By.CSS_SELECTOR, "div[title*='Shopee']"))
                    count_ml = len(driver.find_elements(By.CSS_SELECTOR, "div[title*='Mercado']"))
                    
                    sum_shopee += count_shopee
                    sum_ml += count_ml
                    
                except: continue
            
            STORE_DATA["total_ml"] = sum_ml
            STORE_DATA["total_shopee"] = sum_shopee

            # --- 2. INVENTORY ---
            driver.get("https://app.upseller.com/pt/inventory/list?warehouseId=")
            time.sleep(6)
            driver.execute_script("window.scrollTo(0, 0);")
            
            rows = driver.find_elements(By.TAG_NAME, "tr")

            for row in rows:
                try:
                    row_text = row.text
                    product_key = None
                    
                    # Here is the name of your product (Product 2)
                    if "YOUR PRODUCT NAME 2" in row_text:
                        product_key = "stock_cp2"
                    # Here is the name of your product (Product 1)
                    elif "YOUR PRODUCT NAME 1" in row_text:
                        product_key = "stock_cp1"
                    
                    if product_key:
                        title_items = row.find_elements(By.CSS_SELECTOR, ".f_title")
                        found_values = []
                        for item in title_items:
                            val = item.text.strip()
                            if val.isdigit():
                                found_values.append(val)
                        
                        if found_values:
                            STORE_DATA[product_key] = found_values[-1] 
                except: continue

            # --- 3. FINANCIALS ---
            driver.get("https://app.upseller.com/pt/finance/profit-report/store")
            time.sleep(5)
            
            periods = {
                "Hoje": "profit_today", 
                "7 dias": "profit_7d",      
                "30 dias": "profit_30d",    
                "Este mês": "profit_month"  
            }
            
            try:
                cal_inputs = driver.find_elements(By.CLASS_NAME, "ant-calendar-picker-input")
                if cal_inputs: 
                    force_click(driver, cal_inputs[0])
                    time.sleep(1.5)
            except: pass

            for btn_name, cache_key in periods.items():
                try:
                    xpath_btn = f"//span[contains(@class, 'ant-tag') and contains(text(), '{btn_name}')]"
                    
                    try:
                        elem_btn = driver.find_element(By.XPATH, xpath_btn)
                        if not elem_btn.is_displayed():
                             cal_inputs = driver.find_elements(By.CLASS_NAME, "ant-calendar-picker-input")
                             if cal_inputs: force_click(driver, cal_inputs[0])
                             time.sleep(1)
                        
                        force_click(driver, elem_btn)
                        time.sleep(3)
                        
                        profit_val = driver.find_element(By.CLASS_NAME, "sales_total_title").text
                        STORE_DATA[cache_key] = profit_val.strip()
                        
                    except:
                        cal_inputs = driver.find_elements(By.CLASS_NAME, "ant-calendar-picker-input")
                        if cal_inputs: force_click(driver, cal_inputs[0])
                        time.sleep(1)
                        elem_btn = driver.find_element(By.XPATH, xpath_btn)
                        force_click(driver, elem_btn)
                        time.sleep(3)
                        profit_val = driver.find_element(By.CLASS_NAME, "sales_total_title").text
                        STORE_DATA[cache_key] = profit_val.strip()
                    
                except: continue

        except: pass
        
        print("Searching again in sixty seconds.")
        time.sleep(60)

@app.route('/api/status')
def status():
    return jsonify(STORE_DATA)

if __name__ == '__main__':
    t = threading.Thread(target=bot_manager)
    t.daemon = True
    t.start()
    app.run(host='0.0.0.0', port=5000) # Here you put your IP
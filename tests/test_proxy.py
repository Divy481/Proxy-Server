import sys
import requests
import time
import os

PROXY_PORT = 8081
PROXY_URL = f"http://127.0.0.1:{PROXY_PORT}"

def test_get_request():
    print(f"Testing GET request via proxy {PROXY_URL}...")
    try:
        start_time = time.time()
        proxies = {"http": PROXY_URL, "https": PROXY_URL}
        # Using a reliable site that supports http
        response = requests.get("http://httpbin.org/get", proxies=proxies, timeout=10)
        end_time = time.time()
        
        if response.status_code == 200:
            print(f"[PASS] GET request successful. Time: {end_time - start_time:.4f}s")
            return response.text, end_time - start_time
        else:
            print(f"[FAIL] GET request returned status: {response.status_code}")
            return None, 0
    except Exception as e:
        print(f"[FAIL] GET request failed: {e}")
        return None, 0

def test_caching(initial_content, initial_time):
    print("Testing Caching (Request repetition)...")
    try:
        start_time = time.time()
        proxies = {"http": PROXY_URL, "https": PROXY_URL}
        response = requests.get("http://httpbin.org/get", proxies=proxies, timeout=10)
        end_time = time.time()
        
        duration = end_time - start_time
        
        if response.status_code == 200:
            print(f"[PASS] Second GET request successful. Time: {duration:.4f}s")
            if duration < initial_time:
                 print(f"[INFO] Cache potentially worked (Faster by {initial_time - duration:.4f}s)")
            else:
                 print(f"[INFO] Time difference negligible or slower: {duration:.4f} vs {initial_time:.4f}")
                 
            if response.text == initial_content:
                print("[PASS] Content matches.")
            else:
                print("[FAIL] Content differs (Dynamic content might cause this).")
        else:
            print(f"[FAIL] Second GET request returned status: {response.status_code}")
    except Exception as e:
        print(f"[FAIL] Second GET request failed: {e}")

def test_https_connect():
    print("Testing CONNECT (HTTPS) request...")
    try:
        proxies = {"http": PROXY_URL, "https": PROXY_URL}
        # Verify = False to avoid SSL cert issues if proxied, but for CONNECT tunnel it should be fine.
        response = requests.get("https://reqres.in/api/users/2", proxies=proxies, timeout=10)
        
        if response.status_code == 200:
            print(f"[PASS] HTTPS request successful.")
        else:
            print(f"[FAIL] HTTPS request returned status: {response.status_code}")
    except Exception as e:
        print(f"[FAIL] HTTPS request failed: {e}")

if __name__ == "__main__":
    time.sleep(1) # Wait for server to be ready
    content, duration = test_get_request()
    if content:
        test_caching(content, duration)
    
    test_https_connect()

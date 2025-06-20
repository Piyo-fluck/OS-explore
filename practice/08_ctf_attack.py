import requests
import time

# 正しいベースURL
base_url = "https://lsnl.jp"
problem_url = f"{base_url}/app/ctfx/problem/026052/spit-hornet/os-11-io/21-zoom"
submit_url = f"{base_url}/app/ctfx/submit/026052/spit-hornet/os-11-io/21-zoom"
#spit-hornet/os-11-io/21-zoom

session = requests.Session()
headers = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
    "Content-Type": "application/x-www-form-urlencoded",
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
}

# フォームページを事前に取得（クッキー取得）
try:
    response = session.get(problem_url, headers=headers)
    if response.status_code == 200:
        print("フォームページの取得に成功")
        print("クッキー:", session.cookies.get_dict())
    else:
        print(f"フォームページの取得に失敗: ステータスコード {response.status_code}")
except Exception as e:
    print(f"フォームページの取得中にエラー: {e}")

# 2.01～2.99まで0.01刻みで送信
stst = 1.00
for i in range(999):
    flag = f"{stst:.2f}"
    data = {"flag":flag}
    print(f"\n送信: {flag}")
    try:
        response = session.get(submit_url, params=data, headers=headers)
        if response.status_code == 200:
            if "おめでとう! 正解です。" in response.text:
                print("成功！フラッグ:", flag)
                print(response.text)
                break
            elif "残念ながら不正解です。" in response.text:
                print("不正解")
                stst = stst + 0.01
            else:
                print("判定不能な応答")
        else:
            print(f"送信失敗: ステータスコード {response.status_code}")
            print(response.text)
    except Exception as e:
        print(f"送信エラー: {e}")
    time.sleep(2)
import requests

# 正しいベースURL
base_url = "https://lsnl.jp"
problem_url = f"{base_url}/app/ctfx/problem/026052/dog-sparrow/os-11-io/17-display"
submit_url = f"{base_url}/app/ctfx/submit/026052/dog-sparrow/os-11-io/17-display"

# 送信するデータ（フラッグ）
data = {
    "flag": "1459968"  # 計算したフラッグ（必要に応じて変更）
}

# セッションを作成
session = requests.Session()

# ヘッダーを設定
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
        # クッキーを確認
        print("クッキー:", session.cookies.get_dict())
    else:
        print(f"フォームページの取得に失敗: ステータスコード {response.status_code}")
except Exception as e:
    print(f"フォームページの取得中にエラー: {e}")

# POSTリクエストを試す
print("\nPOSTリクエストを送信中...")
try:
    response = session.post(submit_url, data=data, headers=headers)
    if response.status_code == 200:
        print("送信成功！")
        print(response.text)
    else:
        print(f"送信失敗: ステータスコード {response.status_code}")
        print(response.text)
except Exception as e:
    print(f"POST送信中にエラー: {e}")

# GETリクエストを試す（POSTが失敗した場合）
print("\nGETリクエストを送信中...")
try:
    response = session.get(submit_url, params=data, headers=headers)
    if response.status_code == 200:
        print("送信成功！")
        print(response.text)
        if "残念ながら不正解です。" in response.text:
            print("不正解")
    else:
        print(f"送信失敗: ステータスコード {response.status_code}")
        print(response.text)
except Exception as e:
    print(f"GET送信中にエラー: {e}")
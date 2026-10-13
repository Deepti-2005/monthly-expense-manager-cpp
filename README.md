# Monthly Expense Manager — Browser Demo

This is a Streamlit browser-based companion demo inspired by the features of the original C++17 Monthly Expense Manager repository.

## Run locally
```bash
python -m pip install -r requirements.txt
streamlit run app.py
```

## Deploy
1. Upload `app.py` and `requirements.txt` to a GitHub repository.
2. Open https://share.streamlit.io/
3. Sign in with GitHub and choose the repository, branch, and `app.py`.
4. Click Deploy. Share the generated HTTPS URL.

## Important
This is a separate browser interface, not a direct compilation of the original C++ console application. This demo stores transactions in Streamlit session state; it is intended for classroom demonstration and does not guarantee durable storage after a restart.

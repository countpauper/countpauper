call .venv\Scripts\activate.bat
pip install pyinstaller
rem pyinstaller main.py
pyinstaller -F main.py -n treadle_counter.exe 
rem pyinstaller -F --paths=.venv/Lib/site-packages  main.py
rem pip install pynput==1.6.8

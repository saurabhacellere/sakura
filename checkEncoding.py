﻿from chardet.universaldetector import UniversalDetector
import chardet
import os
import sys
import subprocess

#########################################################
# 定数
#########################################################
patternUTF8_BOM   = (
	"utf-8-sig",
	"ascii"
)
expectEncoding = {
	".cpp" : patternUTF8_BOM,
	".h"   : patternUTF8_BOM,
}

# チェック対象の拡張子リスト
extensions = expectEncoding.keys()

# 指定したファイルの文字コードを返す
def check_encoding(file_path):
	detector = UniversalDetector()
	with open(file_path, mode='rb') as f:
		for binary in f:
			detector.feed(binary)
			if detector.done:
				break
	detector.close()
	return detector.result['encoding']

# チェック対象の拡張子か判断する
def checkExtension(file):
	base, ext = os.path.splitext(file)
	if ext in extensions:
		return True
	else:
		return False

def getMergeBase():
	output = subprocess.check_output('git show-branch --merge-base origin/master HEAD')
	outputDec = output.decode()
	mergeBase = outputDec.splitlines()
	return mergeBase[0]

# ベースとの差分をチェック
def getDiffFiles():
	mergeBase = getMergeBase()

	output = subprocess.check_output('git diff ' + mergeBase + ' --name-only --diff-filter=dr')
	diffFiles = output.decode()
	for file in diffFiles:
		if checkExtension(file):
			yield file

# デバッグ用
# すべてのファイルを対象にチェック対象の拡張子のファイルの文字コードを調べてチェックする
def checkAll():
	for rootdir, dirs, files in os.walk('.'):
		for file in files:
			if checkExtension(file):
				full = os.path.join(rootdir, file)
				yield full

# 指定したファイルの文字コードが期待通りか確認する
def checkEncodingResult(file, encoding):
	base, ext = os.path.splitext(file)
	encoding = encoding.lower()
	if encoding in expectEncoding.get(ext, ()):
		return True
	return False

# 指定されたファイルリストに対して文字コードが適切かチェックする
# (条件に満たないファイル数を返す。)
def processFiles(files):
	# 条件に満たないファイル数
	count = 0

	for file in files:
		encoding = check_encoding(file)
		if not checkEncodingResult(file, encoding):
			print ("NG", encoding, file)
			count = count + 1
		else:
			#print ("OK", encoding, file)
			pass
	return count

if __name__ == '__main__':
	count = 0
	if len(sys.argv) > 1 and sys.argv[1] == "all":
		count = processFiles(checkAll())
	else:
		count = processFiles(getDiffFiles())

	if count > 0:
		print ("return 1")
		sys.exit(1)
	else:
		print ("return 0")
		sys.exit(0)

﻿from chardet.universaldetector import UniversalDetector
import chardet
import os
import sys
import subprocess
import site

#########################################################
# 定数
#########################################################
patternUTF8_BOM   = (
	"utf-8-sig",
	"ascii"
)
patternUTF8_NoBOM = (
	"utf-8"
)
expectEncoding = {
	".cpp" : patternUTF8_BOM,
	".h"   : patternUTF8_BOM,
	".rc"  : patternUTF8_NoBOM,
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
	outputDec = output.decode()
	diffFiles = outputDec.splitlines()
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
def processFiles(files, verbose):
	# 条件に満たないファイル数
	count = 0

	for file in files:
		encoding = check_encoding(file)
		if not checkEncodingResult(file, encoding):
			print ("NG", encoding, file)
			count = count + 1
		else:
			if verbose > 0:
				print ("OK", encoding, file)
			else:
				pass
	return count

if __name__ == '__main__':
	user_scripts = os.path.join(site.USER_BASE, "Scripts")
	sys.path.append(user_scripts)
	print ("adding " + user_scripts + " to PATH")

	verbose = int(os.environ.get("VERBOSE", "0"))
	count = 0
	if len(sys.argv) > 1 and sys.argv[1] == "all":
		count = processFiles(checkAll(), verbose)
	else:
		count = processFiles(getDiffFiles(), verbose)

	if count > 0:
		print ("return 1")
		sys.exit(1)
	else:
		print ("return 0")
		sys.exit(0)

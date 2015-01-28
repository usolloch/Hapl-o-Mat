from collections import defaultdict

def checkLoci(loci):
    for locus in loci:
        if loci.count(locus) != 2:
            print 'Locus ' + locus + ' does nor occur two times.'
            return False
        if not locus.endswith('*'):
            print 'Locus ' + locus + ' misses *.'
            return False
    return True

def checkLastLetter(code, endLetters):
    lastLetter = code[-1]
    preLastLetter = code[-2:-1]
    if lastLetter.isalpha() and not preLastLetter.isalpha():
        if not lastLetter in endLetters:
            print 'Code ' + code + ' in report ' + id + ' ends with not allowed letter.'
            return False
    return True

def checkForLetters(codeWithoutLetterAtTheEnd):
    splittedCodes = codeWithoutLetterAtTheEnd.split(':')
    for splittedCode in splittedCodes:
        if not splittedCode.isdigit():
            print 'Code ' + code + ' in report ' + id + ' contains not valid sign.'
            return False
    return True
   
    
fileName = raw_input('Enter file name:\n')

print 'Check syntax of reports:'

allIds = set()
syntacticallyCleanedReports = [] 
firstLine = ''
loci = []

with open(fileName) as file:
    firstLine = file.readline()
    loci = firstLine.split()
    loci.pop(0)
    checkLoci(loci)
    numberLoci = len(loci)/2

    for line in file:
        reportOkay = True

        codesAtLoci = line.split()
        #check if id already in
        id = codesAtLoci[0]
        if id in allIds:
            print 'Found duplicate of report ' + id
            reportOkay = False
        else:
            allIds.add(id)
        
        codesAtLoci.pop(0)
        #check number codes
        if len(codesAtLoci) / 2. > numberLoci:
            print 'Too many codes in report ' + id
            reportOkay = False
        if len(codesAtLoci) / 2. < numberLoci:
            print 'Too less codes in report ' + id
            reportOkay = False

        for code in codesAtLoci:
            #check for NEW and number colons
            if code == 'NEW':
                print 'Code ' + code + ' in report ' + id + ' is NEW'
                reportOkay = False
            else:
                if code.count(':') < 1:
                    print 'Code ' + code + ' in report ' + id + ' has too less digits.'
                    reportOkay = False
                if code.count(':') > 3:
                    print 'Code ' + code + ' in report ' + id + ' has too many digits.'
                    reportOkay = False

            #check a 4d report
            if code.count(':') == 1:
                endLetters = ('N', 'L', 'S', 'Q', 'g')
                reportOkay = checkLastLetter(code, endLetters)

                codeFirstDigit = code.split(':')[0]
                reportOkay = checkForLetters(codeFirstDigit)

                codeSecondDigit = code.split(':')[1]
                if code[-1].isalpha() and not code[-2].isalpha():
                    codeWithoutLetterAtTheEnd = codeSecondDigit[:-1]
                else:
                    codeWithoutLetterAtTheEnd = codeSecondDigit
                if not codeWithoutLetterAtTheEnd.isdigit():
                    if not codeWithoutLetterAtTheEnd.isalpha():
                        print 'Code ' + code + ' in report ' + id + ' contains not valid sign.'                        
                        reportOkay = False

            #check a 6d report
            elif code.count(':') == 2:
                endLetters = ('N', 'L', 'S', 'Q', 'G')
                reportOkay = checkLastLetter(code, endLetters)
                if code[-1].isalpha() and not code[-2].isalpha():
                    codeWithoutLetterAtTheEnd = code[:-1]
                else:
                    codeWithoutLetterAtTheEnd = code

                reportOkay = checkForLetters(codeWithoutLetterAtTheEnd)

            #check a 8d report
            elif code.count(':') == 3:
                endLetters = ('N', 'L', 'S', 'Q')
                reportOkay = checkLastLetter(code, endLetters)

                reportOkay = checkLastLetter(code, endLetters)
                if code[-1].isalpha() and not code[-2].isalpha():
                    codeWithoutLetterAtTheEnd = code[:-1]
                else:
                    codeWithoutLetterAtTheEnd = code

                reportOkay = checkForLetters(codeWithoutLetterAtTheEnd)

        if reportOkay:
            syntacticallyCleanedReports.append(line)

print '\nCheck consistency of codes:'

code2dna = defaultdict(list)
with open('data/code2dna.txt') as file:
    for line in file:
        line.rstrip('\r\n')
        splittedLine = line.split()
        key = splittedLine[0]
        codes = splittedLine[1]
        splittedCodes = codes.split('/')
        code2dna[key] = splittedCodes
    
expandedAlleles = defaultdict(list)
with open('data/allAllelesExpanded.txt') as file:
    for line in file:
        line.rstrip('\r\n')
        splittedLine = line.split()
        key = splittedLine[0]
        codes = splittedLine[1]
        splittedCodes = codes.split()
        expandedAlleles[key] = codes

H1 = defaultdict(list)
with open('data/H1.txt') as file:
    for line in file:
        line.rstrip('\r\n')
        splittedLine = line.split()
        key = splittedLine[0]
        codes = splittedLine[1]
        splittedCodes = codes.split()
        H1[key] = codesH1 = defaultdict(list)

H1g = defaultdict(list)
with open('data/H1g.txt') as file:
    for line in file:
        line.rstrip('\r\n')
        splittedLine = line.split()
        key = splittedLine[0]
        codes = splittedLine[1]
        splittedCodes = codes.split()
        H1g[key] = codes

completelyCleanedReports = []
for report in syntacticallyCleanedReports:
    reportOkay = True
    codes = report.split()
    codes.pop(0)
    counter = 0
    for code in codes:
        #nmdp
        if code.count(':') == 1 and code.split(':')[1].isalpha():
            if code.split(':')[1] == 'XXX':
                print 'Code ' + code + ' is XXX'
                reportOkay = False
            elif code.split(':')[1] == 'XX':
                print 'Code ' + code + ' is XX'
                reportOkay = False
            elif not code.split(':')[1] in code2dna:
                print 'Code ' + code + ' is not in code2dna.txt.'
                reportOkay = False
            else:
                for combination in code2dna[code.split(':')[1]]:
                    if ':' in combination:
                        resolvedCode = loci[counter] + combination
                    else:
                        resolvedCode = loci[counter] + code.split(':')[0] + ':' + combination
                    if not resolvedCode in expandedAlleles:
                        print 'Code ' + resolvedCode + ' from NMPD-code ' + code + ' is not in allAllelesExpanded.txt.'
                        reportOkay = False
        #G
        elif code.endswith('G'):
            codeWithLocus = loci[counter] + code
            if not codeWithLocus in H1:
                print 'Code ' + codeWithLocus + ' is not in H1.txt.'
                reportOkay = False
        #g
        elif code.endswith('g'):
            codeWithLocus = loci[counter] + code
            if not codeWithLocus in H1g:
                print 'Code ' + codeWithLocus + ' is not in H1g.txt.'
                reportOkay = False
        #allAllelesExpanded
        else:
            codeWithLocus = loci[counter] + code
            if not codeWithLocus in expandedAlleles:
                print 'Code ' + codeWithLocus + ' is not in allAllelesExpanded.txt.'
                reportOkay = False
    
        counter += 1
    if reportOkay:
        completelyCleanedReports.append(report)

        
cleanedFileName = 'cleaned' + fileName
with open(cleanedFileName, 'w') as file:
    file.write(firstLine)
    for report in completelyCleanedReports:
        file.write(report)

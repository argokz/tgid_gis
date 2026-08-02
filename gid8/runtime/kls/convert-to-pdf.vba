' Проверка, что передано два аргумента
If WScript.Arguments.Count < 2 Then
    WScript.Echo "Usage: cscript convert_to_pdf.vbs input.doc output.pdf"
    WScript.Quit 1
End If

' Получение путей к файлам из аргументов
inputFile = WScript.Arguments(0)
outputFile = WScript.Arguments(1)

' Создание объекта Word
Set objWord = CreateObject("Word.Application")

' Открытие документа
Set objDoc = objWord.Documents.Open(inputFile)

' Сохранение документа в формате PDF
objDoc.SaveAs outputFile, 17

' Закрытие документа и завершение Word
objDoc.Close
objWord.Quit

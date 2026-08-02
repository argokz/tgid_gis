param (
    [string]$InputFile,   # Входной файл .doc или .docx
    [string]$OutputFile   # Выходной файл .pdf
)

#Start-Transcript -Path C:\vc_projects\gid6\gidr\kls\3\out.log

# Проверяем, что входной файл существует
if (-not (Test-Path $InputFile)) {
    Write-Host "Входной файл не найден: $InputFile"
    exit 1
}

# Создаем объект Word
$word = New-Object -ComObject Word.Application
$word.Visible = $false

# Открываем документ
$doc = $word.Documents.Open($InputFile)

#$OutputFile = "C:\vc_projects\gid6\gidr\kls\3\qq.pdf"

# Конвертируем в PDF
$doc.SaveAs([ref] $OutputFile, [ref] 17)  # 17 - это формат PDF

# Закрываем документ и Word
$doc.Close()
$word.Quit()

Write-Host "Конвертация завершена: $OutputFile"

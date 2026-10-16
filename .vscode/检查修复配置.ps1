# =============================================================================
#  检查并修复 .vscode 配置（内容 + 只读锁）
#
#  背景：VS Code 的 C/C++ Runner 扩展（franneck94.c-cpp-runner）每次启动都会
#        1) 往 .vscode/settings.json 塞 25 行 C_Cpp_Runner.*
#        2) 把 .vscode/c_cpp_properties.json 改成它自己的 MinGW 配置
#        3) 顺手清掉文件只读属性再写
#        所以单纯上锁会被它破掉，需要用本脚本"检查 + 修复"。
#
#  用法：双击同目录的 检查修复配置.bat，或直接右键本文件"使用 PowerShell 运行"
# =============================================================================

$ErrorActionPreference = 'Stop'
$dir = $PSScriptRoot

$cppJson = @'
{
  "configurations": [
    {
      "name": "C251",
      "includePath": [
        "${workspaceFolder}/**",
        "D:/Keil_v5/C251/INC/**",
        "${workspaceFolder}/Libraries/zf_common",
        "${workspaceFolder}/Libraries/zf_driver",
        "${workspaceFolder}/Libraries/zf_device",
        "${workspaceFolder}/Libraries/zf_components",
        "${workspaceFolder}/Project/user",
        "${workspaceFolder}/Project/code"
      ],
      "defines": [
        "sbit=unsigned char",
        "bit=unsigned char",
        "sfr=unsigned char",
        "sfr16=unsigned short",
        "xdata=",
        "idata=",
        "data=",
        "bdata=",
        "pdata=",
        "code=const",
        "far=",
        "near="
      ],
      "compilerPath": "",
      "cStandard": "c99",
      "intelliSenseMode": "windows-gcc-x86"
    }
  ],
  "version": 4
}
'@

$settingsJson = @'
{
  "files.encoding": "gb2312",
  "files.autoGuessEncoding": true
}
'@

$extensionsJson = @'
{
    "recommendations": [],
    "unwantedRecommendations": [
        "franneck94.c-cpp-runner",
        "formulahendry.code-runner",
        "ms-vscode.cpptools-extension-pack"
    ]
}
'@

$utf8 = New-Object System.Text.UTF8Encoding($false)
$targets = [ordered]@{
    'c_cpp_properties.json' = $cppJson
    'settings.json'         = $settingsJson
    'extensions.json'       = $extensionsJson
}

Write-Host '[1/3] 解锁并还原内容...'
foreach ($name in $targets.Keys) {
    $path = Join-Path $dir $name
    # 只读属性会让写入抛 UnauthorizedAccessException，必须先清掉
    if (Test-Path $path) { attrib -r "$path" 2>$null | Out-Null }
    [System.IO.File]::WriteAllText($path, $targets[$name], $utf8)
    Write-Host ("      {0}  已写入" -f $name)
}

Write-Host '[2/3] 重新上只读锁...'
foreach ($name in $targets.Keys) {
    attrib +r "$(Join-Path $dir $name)" | Out-Null
}

Write-Host '[3/3] 校验...'
$ok = $true
foreach ($name in $targets.Keys) {
    $path = Join-Path $dir $name
    $item = Get-Item $path
    $ro   = [bool]($item.Attributes -band [System.IO.FileAttributes]::ReadOnly)
    try { Get-Content $path -Raw | ConvertFrom-Json | Out-Null; $valid = $true }
    catch { $valid = $false; $ok = $false }
    Write-Host ("      {0,-24} 只读={1,-5} JSON合法={2}" -f $name, $ro, $valid)
    if (-not $ro) { $ok = $false }
}

Write-Host ''
if ($ok) {
    Write-Host '完成：3 个配置文件内容正确且已上锁。' -ForegroundColor Green
} else {
    Write-Host '注意：有文件未锁住（可能 VS Code 正在运行并占用了它），请关闭 VS Code 后重跑本脚本。' -ForegroundColor Yellow
}


var SS = SpreadsheetApp.openById('1XiJtNK_Me9AIQnKF3Z4yVlc5t1mEooSkXBa-wikMkJo');
var sheet = SS.getSheetByName('Sheet1');
var str = "";

function doGet(e){
  var FIELD1 = e.parameter.FIELD1; 
  var FIELD2 = e.parameter.FIELD2; 
  var FIELD3 = e.parameter.FIELD3;
  var FIELD7 = e.parameter.FIELD7;
  var FIELD8 = e.parameter.FIELD8;

  
  if (FIELD1 !== undefined || FIELD2 !== undefined){
    var now = Utilities.formatDate(new Date(), "GMT+7", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11,19);
    var count = (sheet.getRange('G2').getValue());
    if(count == '0'){
      sheet.getRange('E2').setValue(now);
      sheet.getRange('G2').setValue(1);
      
    }else{
      sheet.getRange('F2').setValue(now);
      sheet.getRange('G2').setValue(count+1);
    }

    addProduct(FIELD1, FIELD2 , FIELD3 ,FIELD7,FIELD8 ); 
    
    str = "Sent: "+ now;
    return ContentService.createTextOutput(str);
  }
}

//https://developers.google.com/apps-script/reference/spreadsheet/sheet
function addProduct(FIELD1, FIELD2 , FIELD3 ,FIELD7,FIELD8) {
  var sheet = SpreadsheetApp.getActiveSheet();
  var Time = new Date(); 
  sheet.appendRow([Time, FIELD1, FIELD2 , FIELD3]);
  //sheet.deleteRows(6, 10);
  //Logger.log(values)
  
} 

function fetchDataAndSeparateColumns() {
  // รับข้อมูลทั้งหมดจาก Sheet1
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Sheet1");
  var data = sheet.getRange("A2:D" + sheet.getLastRow()).getValues();

  // สร้าง array เพื่อเก็บข้อมูลที่ถูกดึง
  var extractedData = [];

  for (var i = 99; i < data.length; i += 100) {
    extractedData.push(data[i]);
  }

  // ตรวจสอบว่ามีข้อมูลที่ถูกดึงหรือไม่
  if (extractedData.length > 0) {
    // สร้าง array เพื่อเก็บคอลัมน์ที่แยกแยะ
    var allData = [];

    // วนลูปผ่านข้อมูลที่ถูกดึงและแยกแยะคอลัมน์
    for (var j = 0; j < extractedData.length; j++) {
      allData.push(extractedData[j]);
    }

    // ตรวจสอบว่ามี Sheet ที่ชื่อ "Separated Data" หรือไม่
    var separatedDataSheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Separated Data");
    if (!separatedDataSheet) {
      // ถ้ายังไม่มี, สร้าง Sheet ใหม่
      separatedDataSheet = SpreadsheetApp.getActiveSpreadsheet().insertSheet("Separated Data");
    }

    // นำข้อมูลที่แยกแยะลงใน Sheet "Separated Data"
    separatedDataSheet.getRange(1, 1, allData.length, allData[0].length).setValues(allData);
  } else {
    Logger.log("ไม่มีข้อมูลที่ถูกดึง");
  }
}





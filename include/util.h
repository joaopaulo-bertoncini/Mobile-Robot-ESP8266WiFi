/* Original code on Stackoverflow
	https://stackoverflow.com/questions/29671455/how-to-split-a-string-using-a-specific-delimiter-in-arduino
	Example : 
		String str = "1,2,3";
		String part01 = getValue(str,';',0); // get 1
 		String part02 = getValue(str,';',1); // get 2
		String part03 = getValue(str,';',2); // get 3
		String part03 = getValue(str,';',4); // get empty string
        
	Documented by issa loubani, your average programmer :P 													*/

// Happy coding O.O
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

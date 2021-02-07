package test2x;

/**
 * ...
 * @author Robert Campbell
 */

class TestClass{
	var value:Int = 0;
	var value2:Int = 0;
	
	public function new(v:Int, v2:Int):Void{
		//trace("Constructing TestClass");
		value = v;
		value2 = v2;
	}
	
	public function execute():Void{
		trace("Hello from TestClass::execute");
		trace(value);
		trace(value2);
	}
}
 
class Main
{
	static function main() 
	{
		/*
		for (i in 0...1000000 ){
			trace(i+1);
		var x = new TestClass(10, 30);
		x.execute();
			
		}
		*/
		trace("Hello world");
	}
	
}
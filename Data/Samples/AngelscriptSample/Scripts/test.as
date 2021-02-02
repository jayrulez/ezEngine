void main(){
	Log::Debug("Debug");
	Log::Info("Info");
	Log::Warning("Warning");
	Log::SeriousWarning("SeriousWarning");
	Log::Success("Success");
	Log::Error("Error");
	
	string myLog("Hello world!");
	Log::Debug(myLog);
}
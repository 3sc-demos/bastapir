//
// Copyright 2018 Juraj Durech <durech.juraj@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <bastapir/common/Path.h>
#include <bastapir/BastapirDocument.h>
#include <bastapir/bas/Keywords.h>

using namespace bastapir;
using namespace bastapir::tap;

int main(int argc, const char * argv[])
{
	FileErrorLogger logger;
	BastapirDocument doc(&logger);
	auto path = Path(argv[1]);
	auto file = SourceTextFile(path);
	auto result = doc.processDocument(file);
	if (result) {
		auto bytes = doc.archiveBytes();
		FILE * f = fopen(doc.hasOutputFile() ? doc.outputFile().c_str() : argv[2], "wb");
		if (f) {
			fwrite(bytes.data(), 1, bytes.size(), f);
			fclose(f);
		} else {
			result = false;
		}
	}
	printf("Result: %s\n", result ? "sukcez" : "failure");
	return result ? 0 : 1;
}

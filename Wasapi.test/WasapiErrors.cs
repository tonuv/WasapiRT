using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Wasapi.test
{
    public class WasapiErrors
    {
        public static readonly Int32 AUDCLNT_E_NOT_INITIALIZED = unchecked((int)0x88890001);
        public static readonly Int32 AUDCLNT_E_BUFFER_TOO_LARGE = unchecked((int)0x88890006);
    }
}

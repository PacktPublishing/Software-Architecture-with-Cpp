
### Versioning and deployment philosophy

This library does not maintain "official versions". This is, in part, to avoid the long-standing problem that occurs when, for example, you'd like to use two different libraries in your project, but each of those libraries in turn has a dependency on a different (conflicting) version of a common library (aka "[dependency hell](https://en.wikipedia.org/wiki/Dependency_hell)"). The alternative to maintaining versions is simply a policy of avoiding as much as possible, changes to the library's interface that break existing code that uses the library. So, ideally, any code you write using the library should continue to work with future "versions" of the library. Google calls this approach "[living at head](https://www.youtube.com/watch?v=tISy7EJQPzI)". 

One of the goals of the library is to avoid as much as possible, being a "dependency risk". In fact, rather than a third-party library, you're encouraged to think of the library as, in a sense, just part of your project's source code. In that vein, it may not be a bad idea to have your project's code refer to a more personalized namespace alias, rather than the library's default namespace. 

